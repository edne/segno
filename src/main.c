#include <segno.h>


SCM guile_wrap_shape(Shape shape) {
    Shape *shape_heap = scm_gc_malloc_pointerless(sizeof(Shape), "shape");

    memcpy(shape_heap, &shape, sizeof(Shape));

    return scm_from_pointer(shape_heap, NULL);
}

SCM guile_polyogn(SCM n) {
    Shape polygon = polygon_new(scm_to_int(n));
    return guile_wrap_shape(polygon);
}

SCM guile_scale(SCM shape_scm, SCM ratio) {
    Shape *shape_ref = scm_to_pointer(shape_scm);
    Shape new_shape = shape_scale(*shape_ref,
                                  scm_to_double(ratio));
    return guile_wrap_shape(new_shape);
}

SCM guile_rotate(SCM shape_scm, SCM angle) {
    Shape *shape_ref = scm_to_pointer(shape_scm);
    Shape new_shape = shape_rotate(*shape_ref,
                                   scm_to_double(angle));
    return guile_wrap_shape(new_shape);
}

SCM guile_translate(SCM shape_scm, SCM x, SCM y, SCM z) {
    Shape *shape_ref = scm_to_pointer(shape_scm);
    Shape new_shape = shape_translate(*shape_ref,
                                      scm_to_double(x),
                                      scm_to_double(y),
                                      scm_to_double(z));
    return guile_wrap_shape(new_shape);
}

void *guile_repl(void *v) {
    (void) v;
    scm_init_guile();

    scm_c_eval_string("(use-modules (ice-9 readline))");
    scm_c_eval_string("(activate-readline)");
    scm_shell(0, NULL);
    return NULL;
}

int main() {
    scm_init_guile();

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, guile_repl, NULL);

    scm_c_define_gsubr("polygon",   1, 0, 0, &guile_polyogn);
    scm_c_define_gsubr("scale",     2, 0, 0, &guile_scale);
    scm_c_define_gsubr("rotate",    2, 0, 0, &guile_rotate);
    scm_c_define_gsubr("translate", 4, 0, 0, &guile_translate);

    Context context = gl_init();

    scm_c_primitive_load("init.scm");

    gl_loop(context);
    gl_clean(context);
    return 0;
}
