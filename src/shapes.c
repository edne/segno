#include <segno.h>


SCM scm_from_shape(Shape shape) {
    Shape *shape_heap = scm_gc_malloc_pointerless(sizeof(Shape), "shape");

    memcpy(shape_heap, &shape, sizeof(Shape));

    return scm_from_pointer(shape_heap, NULL);
}

Shape scm_to_shape(SCM shape_scm) {
    Shape *shape_ref = scm_to_pointer(shape_scm);
    Shape shape = *shape_ref;
    return shape;
}

SCM map_transform(SCM (*transform)(SCM, SCM), SCM in_list, SCM value) {
    SCM shape_scm;
    SCM out_list = SCM_EOL;
    SCM env = scm_interaction_environment();

    while (scm_is_pair(in_list)) {
        shape_scm = scm_eval(scm_car(in_list), env);

        out_list = scm_cons(transform(shape_scm, value), out_list);
        in_list = scm_cdr(in_list);
    }
    return out_list;
}

SCM shape_rotate(SCM shape_scm, SCM turns_scm) {
    if (scm_is_pair(shape_scm)) {
        return map_transform(shape_rotate, shape_scm, turns_scm);
    }

    Shape original, out;
    out = original = scm_to_shape(shape_scm);

    double turns = scm_to_double(turns_scm);

    mat4_rotateZ(original.matrix, turns * 2*M_PI, out.matrix);
    return scm_from_shape(out);
}

SCM shape_translate(SCM shape_scm, SCM x_scm) {
    if (scm_is_pair(shape_scm)) {
        return map_transform(shape_translate, shape_scm, x_scm);
    }

    Shape original, out;
    out = original = scm_to_shape(shape_scm);

    double x = scm_to_double(x_scm);
    double v[] = {x, 0, 0};

    mat4_translate(original.matrix, v, out.matrix);
    return scm_from_shape(out);
}

SCM shape_scale(SCM shape_scm, SCM ratio_scm) {
    if (scm_is_pair(shape_scm)) {
        return map_transform(shape_scale, shape_scm, ratio_scm);
    }

    Shape original, out;
    out = original = scm_to_shape(shape_scm);

    double ratio = scm_to_double(ratio_scm);
    double v[] = {ratio, ratio, ratio};

    mat4_scale(original.matrix, v, out.matrix);
    return scm_from_shape(out);
}

SCM polygon_new(SCM n_scm) {
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

    mat4_identity(shape.matrix);

    return scm_from_shape(shape);
}

void shape_draw(SCM shape_scm, Program program) {
    if (scm_is_pair(shape_scm)) {
        SCM list = shape_scm;
        SCM env = scm_interaction_environment();

        while (scm_is_pair(list)) {
            shape_scm = scm_eval(scm_car(list), env);
            shape_draw(shape_scm, program);
            list = scm_cdr(list);
        }
        return;
    }

    Shape shape = scm_to_shape(shape_scm);

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

void guile_bind_primitives() {
    scm_c_define_gsubr("polygon",   1, 0, 0, &polygon_new);
    scm_c_define_gsubr("scale",     2, 0, 0, &shape_scale);
    scm_c_define_gsubr("rotate",    2, 0, 0, &shape_rotate);
    scm_c_define_gsubr("translate", 2, 0, 0, &shape_translate);
}
