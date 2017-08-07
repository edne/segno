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
    glfwMakeContextCurrent(global_window);  // TODO: take a window as argument

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


