#include <segno.h>

typedef struct {
    int n;
    GLuint vertex_buffer;
    GLuint vertex_array;
    GLfloat matrix[16];
} Shape;


void
identity(GLfloat matrix[16]) {
    int i, j;
    for(i=0; i<4; i++)
    for(j=0; j<4; j++)
        matrix[i*4 + j] = (i==j) ? 1 : 0;
}

void
scale(float factor, GLfloat matrix[16]) {
    int i, j;
    for(i=0; i<3; i++)
    for(j=0; j<3; j++)
        matrix[i*4 + j] *= factor;
}


Shape
make_polygon(int n) {
    // Points
    float array[2*n];

    int i;
    float theta;
    for (i=0; i<n; i++) {
        theta = i * 2*M_PI / n - M_PI/2;

        array[i*2] = cos(theta);
        array[i*2 + 1] = sin(theta);
    }
    //

    // OpenGL stuff
    GLuint vbo_point;
    GLuint vao_point;

    glGenBuffers(1, &vbo_point);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_point);
    glBufferData(GL_ARRAY_BUFFER, n*2*sizeof(float), array, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_point);
    glBindVertexArray(vao_point);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_point);
    glVertexAttribPointer(ATTRIB_POINT, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(ATTRIB_POINT);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    //

    // Polygon
    Shape p;
    p.n = n;
    p.vertex_buffer = vbo_point;
    p.vertex_array = vao_point;

    identity(p.matrix);
    scale(0.5, p.matrix);

    return p;
}

void
draw_shape_fill(Shape s) {
    glBindVertexArray(s.vertex_array);
    glDrawArrays(GL_TRIANGLE_FAN, 0, s.n);
    glBindVertexArray(0);
}

void
draw_shape_stroke(Shape s) {
    glBindVertexArray(s.vertex_array);
    glDrawArrays(GL_LINE_LOOP, 0, s.n);
    glBindVertexArray(0);
}

void
free_shape(Shape s) {
    glDeleteVertexArrays(1, &s.vertex_buffer);
    glDeleteBuffers(1, &s.vertex_array);
}

GLFWwindow *
make_window(int w, int h, const char *title) {
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
key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void) scancode;
    (void) mods;
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int
main() {
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
        "uniform mat4 matrix;\n"
        "void main() {\n"
        "    mat2 rotate = mat2(cos(angle), -sin(angle),\n"
        "                       sin(angle), cos(angle));\n"
        "    gl_Position = matrix * vec4(point, 0.0, 1.0);\n"
        "}\n";
    const GLchar *frag_shader =
        "#version 330\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    color = vec4(0.9, 0.9, 0.9, 0);\n"
        "}\n";

    GLuint program = make_program(vert_shader, frag_shader);

    GLint uniform_angle = glGetUniformLocation(program, "angle");
    GLint uniform_matrix = glGetUniformLocation(program, "matrix");

    Shape polygon = make_polygon(6);

    /* Start main loop */
    glfwSetKeyCallback(window, key_callback);
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        double now = glfwGetTime();
        double angle = fmod(now, 2*M_PI);

        glUseProgram(program);

        glUniform1f(uniform_angle, angle);
        glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, polygon.matrix);

        draw_shape_stroke(polygon);

        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    fprintf(stderr, "Exiting ...\n");

    /* Cleanup and exit */
    free_shape(polygon);
    glDeleteProgram(program);

    glfwTerminate();
    return 0;
}
