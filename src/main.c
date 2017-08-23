#include <segno.h>


void *guile_repl(void *v) {
    (void) v;
    scm_init_guile();

    scm_c_eval_string("(use-modules (ice-9 readline))");
    scm_c_eval_string("(activate-readline)");
    scm_shell(0, NULL);
    return NULL;
}

void guile_bind_primitives() {
    scm_c_define_gsubr("polygon",     1, 0, 0, &shape_polygon);

    scm_c_define_gsubr("scale",       0, 0, 1, &transform_scale);
    scm_c_define_gsubr("translate-x", 0, 0, 1, &transform_translate_x);
    scm_c_define_gsubr("translate-y", 0, 0, 1, &transform_translate_y);
    scm_c_define_gsubr("rotate",      0, 0, 1, &transform_rotate);

    scm_c_define_gsubr("transform",   1, 0, 1, &shape_transform);
    scm_c_define_gsubr("combine",     0, 0, 1, &transform_combine);
}

int main() {
    scm_init_guile();

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, guile_repl, NULL);

    guile_bind_primitives();

    Context context = gl_init();

    scm_c_primitive_load("init.scm");

    gl_loop(context);
    gl_clean(context);
    return 0;
}
