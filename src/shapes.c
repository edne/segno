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

SCM scm_from_transform(Transform transform) {
    Transform *transform_heap;
    transform_heap = scm_gc_malloc_pointerless(sizeof(Transform), "transform");

    memcpy(transform_heap, &transform, sizeof(Transform));
    return scm_from_pointer(transform_heap, NULL);
}

Transform scm_to_transform(SCM transform_scm) {
    Transform *transform_ref = scm_to_pointer(transform_scm);
    Transform transform = *transform_ref;
    return transform;
}

SCM apply_transform(SCM shape_scm, SCM transform_scm);

SCM map_transform(SCM transform_scm, SCM in_shapes) {
    SCM shape_scm;
    SCM out_shapes = SCM_EOL;

    while (scm_is_pair(in_shapes)) {
        shape_scm = scm_car(in_shapes);

        out_shapes = scm_cons(apply_transform(shape_scm, transform_scm),
                              out_shapes);
        in_shapes = scm_cdr(in_shapes);
    }
    return out_shapes;
}

SCM apply_transform(SCM shape_scm, SCM transform_scm) {
    if (scm_is_pair(shape_scm)) {
        return map_transform(transform_scm, shape_scm);

    } else if (scm_is_pair(transform_scm)) {
        // Build a list of transformed shapes
        SCM in_transforms = transform_scm;
        SCM out_shapes = SCM_EOL;

        while (scm_is_pair(in_transforms)) {
            transform_scm = scm_car(in_transforms);
            out_shapes = scm_cons(apply_transform(shape_scm, transform_scm),
                                  out_shapes);

            in_transforms = scm_cdr(in_transforms);
        }

        return out_shapes;
    } else {
        Shape original, out;
        out = original = scm_to_shape(shape_scm);

        Transform transform = scm_to_transform(transform_scm);

        mat4x4_mul(out.matrix, transform.matrix, original.matrix);
        return scm_from_shape(out);
    }
}

SCM fork_transform(SCM (*make_transform)(SCM), SCM values_scm) {
    // Build a list of transformations
    SCM out_transforms = SCM_EOL;

    while (scm_is_pair(values_scm)) {
        SCM value_scm = scm_car(values_scm);
        out_transforms = scm_cons(make_transform(value_scm), out_transforms);

        values_scm = scm_cdr(values_scm);
    }
    return out_transforms;
}

SCM make_rotation(SCM values) {
    if (scm_is_pair(values)) {
        return fork_transform(make_rotation, values);

    } else {
        double turns = scm_to_double(values);

        Transform transform;
        mat4x4 identity;
        mat4x4_identity(identity);
        mat4x4_rotate_Z(transform.matrix, identity, turns * 2*M_PI);
        return scm_from_transform(transform);
    }
}

SCM make_tranlsation(SCM values) {
    if (scm_is_pair(values)) {
        return fork_transform(make_tranlsation, values);

    } else {
        double x = scm_to_double(values);

        Transform transform;
        mat4x4_translate(transform.matrix, x, 0, 0);
        return scm_from_transform(transform);
    }
}

SCM make_scaling(SCM values) {
    if (scm_is_pair(values)) {
        return fork_transform(make_scaling, values);

    } else {
        double ratio = scm_to_double(values);

        Transform transform;
        mat4x4 identity;
        mat4x4_identity(identity);
        mat4x4_scale_aniso(transform.matrix, identity, ratio, ratio, ratio);
        return scm_from_transform(transform);
    }
}


SCM shape_rotate(SCM shape_scm, SCM values) {
    SCM transform_scm = make_rotation(values);
    return apply_transform(shape_scm, transform_scm);
}

SCM shape_translate(SCM shape_scm, SCM values) {
    SCM transform_scm = make_tranlsation(values);
    return apply_transform(shape_scm, transform_scm);
}

SCM shape_scale(SCM shape_scm, SCM values) {
    SCM transform_scm = make_scaling(values);
    return apply_transform(shape_scm, transform_scm);
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

    mat4x4_identity(shape.matrix);

    return scm_from_shape(shape);
}

void shape_draw(SCM shape_scm, Program program) {
    if (scm_is_pair(shape_scm)) {
        SCM list = shape_scm;

        while (scm_is_pair(list)) {
            shape_scm = scm_car(list);
            shape_draw(shape_scm, program);
            list = scm_cdr(list);
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

void guile_bind_primitives() {
    scm_c_define_gsubr("polygon",   1, 0, 0, &polygon_new);
    scm_c_define_gsubr("scale",     2, 0, 0, &shape_scale);
    scm_c_define_gsubr("rotate",    2, 0, 0, &shape_rotate);
    scm_c_define_gsubr("translate", 2, 0, 0, &shape_translate);
}
