#include <segno.h>


Shape shape_scale(Shape shape, float factor) {
    Shape out = shape;  // make a copy
    double v[] = {factor, factor, factor};

    mat4_scale(shape.matrix, v, out.matrix);
    return out;
}


Shape shape_rotate(Shape shape, float angle) {
    Shape out = shape;  // make a copy

    mat4_rotateZ(shape.matrix, angle, out.matrix);
    return out;
}


Shape shape_translate(Shape shape, float x, float y, float z) {
    Shape out = shape;  // make a copy
    double v[] = {x, y, z};

    mat4_translate(shape.matrix, v, out.matrix);
    return out;
}


Shape polygon_new(int n) {
    // Points
    float array[2*n];

    int i;
    float theta;
    for (i=0; i<n; i++) {
        theta = i * 2*M_PI / n + M_PI/2;

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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    //

    // Polygon
    Shape shape;
    shape.n = n;
    shape.vertex_buffer = vbo_point;
    shape.vertex_array = vao_point;

    mat4_identity(shape.matrix);

    return shape;
}


void shape_draw(Shape shape, Program program) {
    glUseProgram(program.id);

    // cast to GLfloat array
    GLfloat matrix[16];
    for(int i=0; i<16; i++)
        matrix[i] = shape.matrix[i];

    glUniformMatrix4fv(program.uniform_matrix, 1, GL_FALSE, matrix);

    glBindVertexArray(shape.vertex_array);
    glDrawArrays(GL_LINE_LOOP, 0, shape.n);
    //glDrawArrays(GL_TRIANGLE_FAN, 0, shape.n);

    glBindVertexArray(0);
    glUseProgram(0);
}

void shape_free(Shape shape) {
    glDeleteVertexArrays(1, &shape.vertex_buffer);
    glDeleteBuffers(1, &shape.vertex_array);
}

GLFWwindow *window_new(int w, int h, const char *title) {
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

static void key_callback(GLFWwindow *window,
                         int key, int scancode, int action, int mods) {
    (void) scancode;
    (void) mods;
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
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

void gl_loop(Context context) {
    Shape polygon = polygon_new(6);
    polygon = shape_scale(polygon, 0.75);
    //polygon = shape_rotate(polygon, M_PI/8);
    //polygon = shape_translate(polygon, 0.5, 0, 0);

    glfwSetKeyCallback(context.window, key_callback);
    while (!glfwWindowShouldClose(context.window)) {
        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        shape_draw(polygon, context.program);

        glfwSwapBuffers(context.window);
        glfwPollEvents();
    }

    shape_free(polygon);
}

int main() {
    Context context = gl_init();

    gl_loop(context);
    gl_clean(context);
    return 0;
}
