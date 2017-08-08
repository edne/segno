#include <segno.h>


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

    guile_bind_primitives();

    Context context = gl_init();

    scm_c_primitive_load("init.scm");

    gl_loop(context);
    gl_clean(context);
    return 0;
}
