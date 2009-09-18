#include <cairo.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"
#include "render.h"

render_context *render_context_create(mem_context *context) {
	return calloc(1, sizeof(render_context));
}

render_object *render_object_create(const char *id) {
	render_object *o = calloc(1, sizeof(render_object));
	if(id != 0) {
		o->id = strdup(id);
	}
	return o;
}

void render_object_add_child(render_object *parent, render_object *child) {
	render_object **old = parent->children;

	parent->children = calloc(1, (parent->child_count + 1) *
								 sizeof(render_object*));
	parent->child_count++;

	if(old) {
		memcpy(parent->children, old, sizeof(render_object*) *
			   (parent->child_count - 1));
		free(old);
	}
	parent->children[parent->child_count - 1] = child;
	child->parent = parent;
}

void render_object_remove_child(render_object *parent, render_object *child) {
	render_object **old = parent->children;
	int index;
	for(index=0;index<parent->child_count;++index) {
		if(parent->children[index] == child) {
			break;
		}
	}

	if(index<parent->child_count) {
		child->parent = 0;
		memmove(&parent->children[index],
				&parent->children[index+1],
				(parent->child_count - index - 1) * sizeof(render_object*));
		parent->child_count--;
		if(parent->child_count == 0) {
			parent->children = 0;
		}
		else {
			parent->children = calloc(parent->child_count, sizeof(render_object*));
			memcpy(parent->children, old, parent->child_count *
				   sizeof(render_object*));
		}
		free(old);
	}
}

render_object *render_object_find(render_object *parent, const char *id) {
	int index;
	render_object *object;
	if(parent->id != 0) {
		if(strcmp(parent->id, id) == 0) {
			return parent;
		}
	}

	for(index=0;index<parent->child_count;++index) {
		if(parent->children[index]->id != 0) {
			if(strcmp(parent->children[index]->id, id) == 0) {
				return parent->children[index];
			}

			object = render_object_find(parent->children[index], id);
			if(object != 0) {
				return object;
			}
		}
	}
	return 0;
}

render_object *render_object_find_root(render_object *object) {
	if(object->parent == 0) {
		return object;
	}

	return render_object_find_root(object->parent);
}

void render_scene_object(
	render_context *rcontext, render_object *object, float delta) {
	int i;
	object->render(rcontext, object, delta);
	for(i=0;i<object->child_count;++i) {
		render_scene_object(rcontext, object->children[i], delta);
	}
}

void render_scene_context(render_context *rcontext, float delta) {
	render_selection_reset(rcontext);
	if(rcontext->object) {
		render_scene_object(rcontext, rcontext->object, delta);
	}
}

GLuint render_register_selection_callback(
	render_context *rcontext, render_object *object,
	render_selection_callback callback, void *data) {

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

void render_process_selections(
	render_context *rcontext, GLint hits, GLuint* selections) {
	int index, hit, records;
	bool hit_found = false;
	GLuint last_hit=0;

	for(index=0, hit=0;hit<hits;++hit) {
		records = selections[index];
		if(records > 0) {
			last_hit = selections[index + 2 + records];
			hit_found = true;
		}
		index += records + 3;
	}

	if(hit_found) {
		rcontext->selections[last_hit].callback(
			rcontext->selections[last_hit].object,
			rcontext->selections[last_hit].data);
	}
}

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
    RsvgDimensionData dimensions;
	RsvgPositionData pos;
	cairo_matrix_t matrix;
	int stride;
	unsigned char* cairo_data;
	cairo_surface_t *cairo_surface;
	cairo_t *cr;
	double xzoom=1.0f, yzoom=1.0f;

	if(!rsvg_handle_has_sub(h, node_name)) {
		fprintf(stderr, "%s wasn't found within the svg document.\n", node_name);
		exit(0);
	}

	if(!rsvg_handle_get_dimensions_sub(h, &dimensions, node_name)) {
		fprintf(stderr, "Failed to obtain the card dimensions for: %s\n",
				node_name);
		exit(0);
	}

	rsvg_handle_get_position_sub(h, &pos, node_name);
	if (dimensions.width <= 0 || dimensions.height <= 0) {
		exit(0);
	}
	stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
	cairo_data = (unsigned char *) calloc(stride * height, 1);
	cairo_surface = cairo_image_surface_create_for_data(
		cairo_data, CAIRO_FORMAT_ARGB32, width,	height, stride);

	cr = cairo_create(cairo_surface);
	cairo_set_source_rgba (cr, 1.0f, 1.0f, 1.0f, 1.0f);
	cairo_rectangle (cr, 0, 0, width, height);
	cairo_fill (cr);

	xzoom = (double)width / (double)dimensions.width;
	yzoom = (double)height / (double)dimensions.height;

	cairo_matrix_init_identity(&matrix);
	cairo_matrix_scale(&matrix, xzoom, yzoom);
	cairo_matrix_translate(&matrix, 0 - pos.x, 0 - pos.y);

	cairo_set_matrix(cr, &matrix);

	if(!rsvg_handle_render_cairo_sub(h, cr, node_name)) {
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
				 0, GL_BGRA, GL_UNSIGNED_BYTE, cairo_data);

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
