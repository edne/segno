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

SCM apply_transform(SCM shape, SCM transform);

SCM map_transform(SCM transform, SCM in_shapes) {
    SCM shape;
    SCM out_shapes = SCM_EOL;

    foreach(shape, in_shapes) {
        out_shapes = scm_cons(apply_transform(shape, transform),
                              out_shapes);
    }
    return out_shapes;
}

SCM apply_transform(SCM shape, SCM transform) {
    if (scm_is_pair(shape)) {
        return map_transform(transform, shape);

    } else if (scm_is_pair(transform)) {
        // Build a list of transformed shapes
        SCM in_transforms = transform;
        SCM out_shapes = SCM_EOL;

        foreach(transform, in_transforms) {
            out_shapes = scm_cons(apply_transform(shape, transform),
                                  out_shapes);
        }

        return out_shapes;

    } else {
        Shape original, out;
        out = original = scm_to_shape(shape);

        Transform transform_unpacked = scm_to_transform(transform);

        mat4x4_mul(out.matrix, transform_unpacked.matrix, original.matrix);
        return scm_from_shape(out);
    }
}

SCM transform_combine_two(SCM target, SCM source) {
    if (scm_is_pair(target)) {
        SCM out = SCM_EOL;

        SCM transform;
        foreach(transform, target) {
            out = scm_cons(transform_combine_two(transform, source),
                           out);
        }
        return out;
    }

    if (scm_is_pair(source)) {
        SCM out = SCM_EOL;

        SCM transform;
        foreach(transform, source) {
            out = scm_cons(transform_combine_two(target, transform),
                           out);
        }
        return out;
    }

    Transform source_unpacked, target_unpacked;

    source_unpacked = scm_to_transform(source);
    target_unpacked = scm_to_transform(target);

    mat4x4_mul(target_unpacked.matrix,
               source_unpacked.matrix,
               target_unpacked.matrix);

    return scm_from_transform(target_unpacked);
}

SCM transform_combine(SCM transforms) {
    SCM source, target;

    Transform identity;
    mat4x4_identity(identity.matrix);
    target = scm_from_transform(identity);

    foreach(source, transforms) {
        target = transform_combine_two(target, source);
    }

    return target;
}

SCM shape_change(SCM shape, SCM transform) {
    // for now just one transformation
    return apply_transform(shape, transform_combine(transform));
}

SCM fork_transform(SCM (*make_transform)(SCM), SCM values) {
    // Build a list of transformations
    SCM out_transforms = SCM_EOL;

    SCM value;
    foreach(value, values) {
        out_transforms = cons(make_transform(value), out_transforms);
    }
    return out_transforms;
}

SCM transform_rotate(SCM values) {
    if (scm_is_pair(values)) {
        return fork_transform(transform_rotate, values);

    } else {
        double turns = scm_to_double(values);

        Transform transform;
        mat4x4 identity;
        mat4x4_identity(identity);
        mat4x4_rotate_Z(transform.matrix, identity, turns * 2*M_PI);
        return scm_from_transform(transform);
    }
}

SCM transform_translate(SCM values) {
    if (scm_is_pair(values)) {
        return fork_transform(transform_translate, values);

    } else {
        double x = scm_to_double(values);

        Transform transform;
        mat4x4_translate(transform.matrix, x, 0, 0);
        return scm_from_transform(transform);
    }
}

SCM transform_scale(SCM values) {
    if (scm_is_pair(values)) {
        return fork_transform(transform_scale, values);

    } else {
        double ratio = scm_to_double(values);

        Transform transform;
        mat4x4 identity;
        mat4x4_identity(identity);
        mat4x4_scale_aniso(transform.matrix, identity, ratio, ratio, ratio);
        return scm_from_transform(transform);
    }
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

void guile_bind_primitives() {
    scm_c_define_gsubr("polygon",   1, 0, 0, &polygon_new);

    scm_c_define_gsubr("scale",     0, 0, 1, &transform_scale);
    scm_c_define_gsubr("translate", 0, 0, 1, &transform_translate);
    scm_c_define_gsubr("rotate",    0, 0, 1, &transform_rotate);

    scm_c_define_gsubr("change",    1, 0, 1, &shape_change);
    scm_c_define_gsubr("combine",   0, 0, 1, &transform_combine);
}
