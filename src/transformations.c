#include <segno.h>

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

SCM transform_map(SCM transform, SCM in_shapes) {
    SCM shape;
    SCM out_shapes = SCM_EOL;

    foreach(shape, in_shapes) {
        out_shapes = scm_cons(transform_apply(shape, transform),
                              out_shapes);
    }
    return out_shapes;
}

SCM transform_apply(SCM shape, SCM transform) {
    if (scm_is_pair(shape)) {
        return transform_map(transform, shape);

    } else if (scm_is_pair(transform)) {
        // Build a list of transformed shapes
        SCM in_transforms = transform;
        SCM out_shapes = SCM_EOL;

        foreach(transform, in_transforms) {
            out_shapes = scm_cons(transform_apply(shape, transform),
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

SCM transform_fork(SCM (*make_transform)(SCM), SCM values) {
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
        return transform_fork(transform_rotate, values);

    } else {
        double turns = scm_to_double(values);

        Transform transform;
        mat4x4 identity;
        mat4x4_identity(identity);
        mat4x4_rotate_Z(transform.matrix, identity, turns * 2*M_PI);
        return scm_from_transform(transform);
    }
}

SCM transform_translate_x(SCM values) {
    if (scm_is_pair(values)) {
        return transform_fork(transform_translate_x, values);

    } else {
        double x = scm_to_double(values);

        Transform transform;
        mat4x4_translate(transform.matrix, x, 0, 0);
        return scm_from_transform(transform);
    }
}


SCM transform_translate_y(SCM values) {
    if (scm_is_pair(values)) {
        return transform_fork(transform_translate_y, values);

    } else {
        double y = scm_to_double(values);

        Transform transform;
        mat4x4_translate(transform.matrix, 0, y, 0);
        return scm_from_transform(transform);
    }
}


SCM transform_scale(SCM values) {
    if (scm_is_pair(values)) {
        return transform_fork(transform_scale, values);

    } else {
        double ratio = scm_to_double(values);

        Transform transform;
        mat4x4 identity;
        mat4x4_identity(identity);
        mat4x4_scale_aniso(transform.matrix, identity, ratio, ratio, ratio);
        return scm_from_transform(transform);
    }
}

