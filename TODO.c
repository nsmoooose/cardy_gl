#if 0

typedef struct render_object_St {
	/** An id that describes this object. */
	char *id;

	/* Custom data for this rendering object. */
	void *data;

	/* Render function. */
	void (*render)(struct render_object_St *object);

	/* Activate function when user clicks on this object. */
	void (*activate)(struct render_object_St *object);

	struct render_object_St **children;
	int child_count;
} render_object;

typedef struct {
	render_object *object;
} render_context;

render_context *render_context_create(mem_context *context);
void render_object_create(mem_context *context, char *id);
void render_object_add_child(mem_context *context, render_object *parent, render_object *child);
void render_object_remove_child(mem_context *context, render_object *parent, render_object *child);
render_object *render_object_find(render_object *root, char *id);
void render_scene(render_context *rcontext);


render_object *render_object_desktop(context);
render_object *render_object_main_menu(context);


void foo() {
	/* Memory context */
	mem_context *context = mem_context_create();

	/* Rendering context. Root of all things. */
	render_context *rcontext = render_context_create(context);

	/* The desktop is the background of the entire application. Let it be the root. */
	render_object *desktop = render_object_desktop(context);
	rcontext->object = desktop;

	render_object *main_menu = render_object_main_menu(context);
	render_object_add_child(context, desktop, main_menu);
}

#endif
