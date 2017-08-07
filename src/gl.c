#include <segno.h>


GLFWwindow *window_new(int w, int h, const char *title) {
    GLFWwindow *window = glfwCreateWindow(w, h, title, NULL, NULL);
    global_window = window;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // vsync

    // TODO: if multiple windows use a static flag to call this just once
    if (gl3wInit()) {
        fprintf(stderr, "gl3w: failed to initialize\n");
        exit(EXIT_FAILURE);
    }

    return window;
}

Context gl_init() {
    if (!glfwInit()) {
        fprintf(stderr, "GLFW3: failed to initialize\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Shader sources */
    const GLchar *vert_shader =
        "#version 330\n"
        "layout(location = 0) in vec2 point;\n"
        "uniform mat4 matrix;\n"
        "void main() {\n"
        "    gl_Position = matrix * vec4(point, 0.0, 1.0);\n"
        "}\n";
    const GLchar *frag_shader =
        "#version 330\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    color = vec4(0.9, 0.9, 0.9, 0);\n"
        "}\n";

    Context context;
    context.window = window_new(640, 640, "OpenGL 3.3 Demo");
    context.program = program_new(vert_shader, frag_shader);

    return context;
}

void gl_clean(Context context) {
    program_free(context.program);
    glfwTerminate();
}

static void key_callback(GLFWwindow *window,
                         int key, int scancode, int action, int mods) {
    (void) scancode;
    (void) mods;
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void gl_loop(Context context) {
    glfwSetKeyCallback(context.window, key_callback);
    while (!glfwWindowShouldClose(context.window)) {
        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        SCM guile_polygon;
        Shape *polygon_ref;

        guile_polygon = scm_c_eval_string("(get-root-shape)");
        polygon_ref = scm_to_pointer(guile_polygon);

        shape_draw(*polygon_ref, context.program);

        glfwSwapBuffers(context.window);
        glfwPollEvents();
    }
}
