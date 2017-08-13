#include <segno.h>


SCM scm_from_shape(Shape shape) {
    Shape *shape_heap;
    shape_heap = scm_gc_malloc_pointerless(sizeof(Shape), "shape");

    memcpy(shape_heap, &shape, sizeof(Shape));
    return scm_from_pointer(shape_heap, NULL);
}

Shape scm_to_shape(SCM shape_scm) {
    Shape *shape_ref = scm_to_pointer(shape_scm);
    Shape shape = *shape_ref;
    return shape;
}

SCM shape_change(SCM shape, SCM transform) {
    // for now just one transformation
    return transform_apply(shape, transform_combine(transform));
}

SCM shape_polygon(SCM n_scm) {
    int n = scm_to_int(n_scm);
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

    mat4x4_identity(shape.matrix);

    return scm_from_shape(shape);
}

void shape_draw(SCM shape_scm, Program program) {
    if (scm_is_pair(shape_scm)) {
        SCM list = shape_scm;

        SCM shape_scm;
        foreach(shape_scm, list)
        {
            shape_scm = first(list);
            shape_draw(shape_scm, program);
        }

        return;
    }

    Shape shape = scm_to_shape(shape_scm);

    glUseProgram(program.id);

    // cast to GLfloat array
    GLfloat matrix[16];
    for(int i=0; i<4; i++)
    for(int j=0; j<4; j++)
        matrix[i*4 + j] = shape.matrix[i][j];

    glUniformMatrix4fv(program.uniform_matrix, 1, GL_FALSE, matrix);

    glBindVertexArray(shape.vertex_array);
    glDrawArrays(GL_LINE_LOOP, 0, shape.n);
    //glDrawArrays(GL_TRIANGLE_FAN, 0, shape.n);

    glBindVertexArray(0);
    glUseProgram(0);
}
