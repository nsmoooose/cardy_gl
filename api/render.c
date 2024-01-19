#include <cairo.h>
#include <librsvg/rsvg.h>
#include <stdlib.h>
#include <string.h>
#include "api/card.h"
#include "api/render.h"

render_context *render_context_create(void) {
	return calloc(1, sizeof(render_context));
}

void render_context_free(render_context *rcontext) {
	if (rcontext != 0) {
		if (rcontext->object != 0) {
			render_object_free(rcontext, rcontext->object);
		}
		free(rcontext);
	}
}

render_object *render_object_create(const char *id) {
	render_object *o = calloc(1, sizeof(render_object));
	if (id != 0) {
		o->id = strdup(id);
	}
	return o;
}

void render_object_free(render_context *rcontext, render_object *object) {
	render_event_args event;

	event.rcontext = rcontext;

	if (object != 0) {
		int index;
		if (object->parent != 0) {
			render_object_remove_child(object);
		}

		for (index = 0; index < object->child_count; ++index) {
			render_object_free(rcontext, object->children[index]);
		}

		event.object = object;
		if (object->free != 0) {
			object->free(&event);
		}
		free(object);
	}
}

void render_object_queue_free(render_context *rcontext, render_object *object) {
	for (int i = 0; i < QUEUE_FREE_SIZE; i++) {
		if (rcontext->queue_free[i] == NULL) {
			rcontext->queue_free[i] = object;
			return;
		}
	}
	exit(1);
}

void render_object_queue_free_children(render_context *rcontext,
                                       render_object *object) {
	for (int i = 0; i < object->child_count; ++i) {
		render_object_queue_free(rcontext, object->children[i]);
	}
}

void render_object_add_child(render_object *parent, render_object *child) {
	render_object **old = parent->children;

	parent->children =
		calloc(1, (parent->child_count + 1) * sizeof(render_object *));
	parent->child_count++;

	if (old) {
		memcpy(parent->children, old,
		       sizeof(render_object *) * (parent->child_count - 1));
		free(old);
	}
	parent->children[parent->child_count - 1] = child;
	child->parent = parent;
}

void render_object_remove_child(render_object *child) {
	render_object *parent = child->parent;
	render_object **old = parent->children;
	int index;
	for (index = 0; index < parent->child_count; ++index) {
		if (parent->children[index] == child) {
			break;
		}
	}

	if (index < parent->child_count) {
		child->parent = 0;
		memmove(&parent->children[index], &parent->children[index + 1],
		        (parent->child_count - index - 1) * sizeof(render_object *));
		parent->child_count--;
		if (parent->child_count == 0) {
			parent->children = 0;
		} else {
			parent->children =
				calloc(parent->child_count, sizeof(render_object *));
			memcpy(parent->children, old,
			       parent->child_count * sizeof(render_object *));
		}
		free(old);
	}
}

render_object *render_object_find(render_object *parent, const char *id) {
	int index;
	render_object *object;
	if (parent->id != 0) {
		if (strcmp(parent->id, id) == 0) {
			return parent;
		}
	}

	for (index = 0; index < parent->child_count; ++index) {
		if (parent->children[index]->id != 0) {
			if (strcmp(parent->children[index]->id, id) == 0) {
				return parent->children[index];
			}

			object = render_object_find(parent->children[index], id);
			if (object != 0) {
				return object;
			}
		}
	}
	return 0;
}

render_object *render_object_find_root(render_object *object) {
	if (object->parent == 0) {
		return object;
	}

	return render_object_find_root(object->parent);
}

void render_scene_object(render_context *rcontext, render_object *object,
                         float delta) {
	int i;
	render_event_args event;
	event.rcontext = rcontext;
	event.object = object;

	if (object->render) {
		object->render(&event, delta);
	}

	for (i = 0; i < object->child_count; ++i) {
		render_scene_object(rcontext, object->children[i], delta);
	}

	if (object->post_render) {
		object->post_render(&event, delta);
	}

	if (object->first_frame_rendered) {
		object->render_time += delta;
	} else {
		object->first_frame_rendered = true;
	}
}

void render_scene_context(render_context *rcontext) {
	float delta;
	int current_time = glutGet(GLUT_ELAPSED_TIME);

	delta = ((float)(current_time - rcontext->last_render)) / 1000.0f;

	render_selection_reset(rcontext);
	if (rcontext->object) {
		render_scene_object(rcontext, rcontext->object, delta);
	}

	rcontext->last_render = current_time;

	for (int i = 0; i < QUEUE_FREE_SIZE; i++) {
		if (rcontext->queue_free[i]) {
			render_object_free(rcontext, rcontext->queue_free[i]);
			rcontext->queue_free[i] = NULL;
		} else {
			break;
		}
	}
}

GLuint render_register_selection_callback(render_context *rcontext,
                                          render_object *object,
                                          render_selection_callback callback,
                                          void *data) {

	GLuint i = rcontext->selection_size;
	rcontext->selections[i].object = object;
	rcontext->selections[i].callback = callback;
	rcontext->selections[i].data = data;
	rcontext->selection_size++;
	return i;
}

void render_selection_reset(render_context *rcontext) {
	memset(rcontext->selections, 0, sizeof(rcontext->selections));
	rcontext->selection_size = 0;
}

void render_process_selections(render_context *rcontext, GLint hits,
                               GLuint *selections) {
	int index, hit, records;
	bool hit_found = false;
	GLuint last_hit = 0;
	render_event_args event;

	for (index = 0, hit = 0; hit < hits; ++hit) {
		records = selections[index];
		if (records > 0) {
			last_hit = selections[index + 2 + records];
			hit_found = true;
		}
		index += records + 3;
	}

	if (hit_found) {
		event.rcontext = rcontext;
		event.object = rcontext->selections[last_hit].object;
		rcontext->selections[last_hit].callback(
			&event, rcontext->selections[last_hit].data);
	}
}

RsvgHandle *render_svg_open(const char *path) {
	GError *e = 0;
	RsvgHandle *h;

	h = rsvg_handle_new_from_file(path, &e);
	if (e != 0 || h == 0) {
		fprintf(stderr, "Failed to open file: %s\n", path);
		return 0;
	}

	return h;
}

void render_svg_close(RsvgHandle *h) { g_object_unref(h); }

void render_svg_texture(RsvgHandle *h, GLuint texture, char *node_name,
                        int width, int height) {
	/*
	  This can be some source of information on how to render a svg file
	  into a bitmap. This is the svg -> bitmap conversion tool part of
	  gnome.

	  git clone git://git.gnome.org/librsvg

	  This file contains information about how to render things into bitmaps:
	    rsvg-convert.c
	*/
	int stride;
	unsigned char *cairo_data;
	cairo_surface_t *cairo_surface;
	cairo_t *cr;

	if (!rsvg_handle_has_sub(h, node_name)) {
		fprintf(stderr, "%s wasn't found within the svg document.\n",
		        node_name);
		exit(0);
	}

	GError *e = 0;
	RsvgRectangle ink_rect, logical_rect;
	if (!rsvg_handle_get_geometry_for_element(h, node_name, &ink_rect,
	                                          &logical_rect, &e)) {
		fprintf(stderr, "Failed to obtain the card dimensions for: %s\n",
		        node_name);
		exit(0);
	}

	/* 	fprintf(stderr, "%s\n", node_name);
	        fprintf(stderr, "Output dimensions: %d x %d\n", width, height);
	        fprintf(stderr, "Sizes: \n  Ink: %f x %f\n  Logical: %f x %f\n",
	   ink_rect.width, ink_rect.height, logical_rect.width,
	   logical_rect.height); fprintf(stderr, "Position: \n  Ink: %f x %f\n
	   Logical: %f x %f\n", ink_rect.x, ink_rect.y, logical_rect.x,
	   logical_rect.y);
	 */
	stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
	cairo_data = (unsigned char *)calloc(stride * height, 1);
	cairo_surface = cairo_image_surface_create_for_data(
		cairo_data, CAIRO_FORMAT_ARGB32, width, height, stride);

	cr = cairo_create(cairo_surface);
	cairo_set_source_rgba(cr, 1.0f, 1.0f, 1.0f, 1.0f);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);

	/* rsvg_handle_render_element will make sure that the vector is zoomed
	   in to fit into the bitmap. But it will keep the aspect ratio of the
	   svg. Adjust a single axis depending on the aspect ratio differences. */
	double xzoom = 1;
	double yzoom = 1;
	double ar1 = (double)width / (double)height;
	double ar2 = ink_rect.width / ink_rect.height;
	if (ar1 > ar2) {
		xzoom = ar1 / ar2;
	} else {
		yzoom = ar2 / ar1;
	}

	cairo_matrix_t matrix;
	cairo_matrix_init_identity(&matrix);
	cairo_matrix_scale(&matrix, xzoom, yzoom);
	/* cairo_matrix_translate(&matrix, 0 - ink_rect.x, 0 - ink_rect.y); */
	cairo_set_matrix(cr, &matrix);

	RsvgRectangle viewport = {0, 0, width, height};
	if (!rsvg_handle_render_element(h, cr, node_name, &viewport, &e)) {
		fprintf(stderr, "Failed to render image: %s.\n", node_name);
		exit(0);
	}

	/*
	char filename_buffer[24];
	snprintf(filename_buffer, sizeof(filename_buffer), "tmp/%s.png", node_name);
	cairo_surface_write_to_png (cairo_surface, filename_buffer);
	*/

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA,
	             GL_UNSIGNED_BYTE, cairo_data);

	free(cairo_data);
	cairo_destroy(cr);
	cairo_surface_destroy(cairo_surface);
}

void render_rect(float x1, float y1, float x2, float y2, GLuint texture) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(x1, y1);

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(x2, y1);

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(x2, y2);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(x1, y2);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

bool render_process_keyboard_down_iterator(render_context *rcontext,
                                           render_object *object,
                                           unsigned char key, int modifiers,
                                           int x, int y) {
	render_event_args event;
	int i;
	bool result;

	for (i = 0; i < object->child_count; ++i) {
		result = render_process_keyboard_down_iterator(
			rcontext, object->children[i], key, modifiers, x, y);

		if (result) {
			return true;
		}
	}

	if (object->keyboard_down) {
		event.rcontext = rcontext;
		event.object = object;
		return object->keyboard_down(&event, key, modifiers, x, y);
	}
	return false;
}

void render_process_keyboard_down(render_context *rcontext, unsigned char key,
                                  int modifiers, int x, int y) {
	render_process_keyboard_down_iterator(rcontext, rcontext->object, key,
	                                      modifiers, x, y);
}

bool render_process_keyboard_up_iterator(render_context *rcontext,
                                         render_object *object,
                                         unsigned char key, int modifiers,
                                         int x, int y) {
	render_event_args event;
	int i;
	bool result;

	for (i = 0; i < object->child_count; ++i) {
		result = render_process_keyboard_up_iterator(
			rcontext, object->children[i], key, modifiers, x, y);

		if (result) {
			return true;
		}
	}

	if (object->keyboard_up) {
		event.rcontext = rcontext;
		event.object = object;
		return object->keyboard_up(&event, key, modifiers, x, y);
	}
	return false;
}

void render_process_keyboard_up(render_context *rcontext, unsigned char key,
                                int modifiers, int x, int y) {
	render_process_keyboard_up_iterator(rcontext, rcontext->object, key,
	                                    modifiers, x, y);
}

bool render_process_keyboard_special_down_iterator(render_context *rcontext,
                                                   render_object *object,
                                                   int key, int modifiers,
                                                   int x, int y) {
	render_event_args event;
	int i;
	bool result;

	for (i = 0; i < object->child_count; ++i) {
		result = render_process_keyboard_special_down_iterator(
			rcontext, object->children[i], key, modifiers, x, y);

		if (result) {
			return true;
		}
	}

	if (object->keyboard_special_down) {
		event.rcontext = rcontext;
		event.object = object;
		return object->keyboard_special_down(&event, key, modifiers, x, y);
	}
	return false;
}

void render_process_keyboard_special_down(render_context *rcontext, int key,
                                          int modifiers, int x, int y) {
	render_process_keyboard_special_down_iterator(rcontext, rcontext->object,
	                                              key, modifiers, x, y);
}

bool render_process_keyboard_special_up_iterator(render_context *rcontext,
                                                 render_object *object, int key,
                                                 int modifiers, int x, int y) {
	render_event_args event;
	int i;
	bool result;

	for (i = 0; i < object->child_count; ++i) {
		result = render_process_keyboard_special_up_iterator(
			rcontext, object->children[i], key, modifiers, x, y);

		if (result) {
			return true;
		}
	}

	if (object->keyboard_special_up) {
		event.rcontext = rcontext;
		event.object = object;
		return object->keyboard_special_up(&event, key, modifiers, x, y);
	}
	return false;
}

void render_process_keyboard_special_up(render_context *rcontext, int key,
                                        int modifiers, int x, int y) {
	render_process_keyboard_special_up_iterator(rcontext, rcontext->object, key,
	                                            modifiers, x, y);
}
