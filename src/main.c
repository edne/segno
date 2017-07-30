#include <segno.h>

GLFWwindow *
make_window(int w, int h, const char *title)
{
    GLFWwindow *window = glfwCreateWindow(w, h, title, NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // vsync

    // TODO: if multiple windows use a static flag to call this just once
    if (gl3wInit()) {
        fprintf(stderr, "gl3w: failed to initialize\n");
        exit(EXIT_FAILURE);
    }

    return window;
}

static void
key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void) scancode;
    (void) mods;
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int
main()
{
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

    GLFWwindow *window = make_window(640, 640, "OpenGL 3.3 Demo");

    /* Shader sources */
    const GLchar *vert_shader =
        "#version 330\n"
        "layout(location = 0) in vec2 point;\n"
        "uniform float angle;\n"
        "void main() {\n"
        "    mat2 rotate = mat2(cos(angle), -sin(angle),\n"
        "                       sin(angle), cos(angle));\n"
        "    gl_Position = vec4(0.75 * rotate * point, 0.0, 1.0);\n"
        "}\n";
    const GLchar *frag_shader =
        "#version 330\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    color = vec4(1, 0.15, 0.15, 0);\n"
        "}\n";

    GLuint program = make_program(vert_shader, frag_shader);

    GLint uniform_angle = glGetUniformLocation(program, "angle");


    const float SQUARE[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f
    };

    GLuint vbo_point;
    GLuint vao_point;

    glGenBuffers(1, &vbo_point);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_point);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SQUARE), SQUARE, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_point);
    glBindVertexArray(vao_point);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_point);
    glVertexAttribPointer(ATTRIB_POINT, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(ATTRIB_POINT);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    /* Start main loop */
    glfwSetKeyCallback(window, key_callback);
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.15, 0.15, 0.15, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        double now = glfwGetTime();
        double angle = fmod(now, 2*M_PI);

        glUseProgram(program);

        glUniform1f(uniform_angle, angle);

        glBindVertexArray(vao_point);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, countof(SQUARE) / 2);
        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    fprintf(stderr, "Exiting ...\n");

    /* Cleanup and exit */
    glDeleteVertexArrays(1, &vao_point);
    glDeleteBuffers(1, &vbo_point);
    glDeleteProgram(program);

    glfwTerminate();
    return 0;
}
