#pragma once

#include "types.h"

float _length_vec2(vec2 vec);
float _length_vec3(vec3 vec);
float _length_vec4(vec4 vec);
#define length(vec) _Generic((vec), \
    vec2: _length_vec2, \
    vec3: _length_vec3, \
    vec4: _length_vec4 \
)(vec)

vec2 _normalize_vec2(vec2 vec);
vec3 _normalize_vec3(vec3 vec);
vec4 _normalize_vec4(vec4 vec);
#define normalize(vec) _Generic((vec), \
    vec2: _normalize_vec2, \
    vec3: _normalize_vec3, \
    vec4: _normalize_vec4 \
)(vec)

vec2 _negate_vec2(vec2 vec);
vec3 _negate_vec3(vec3 vec);
vec4 _negate_vec4(vec4 vec);
#define negate(vec) _Generic((vec), \
    vec2: _negate_vec2, \
    vec3: _negate_vec3, \
    vec4: _negate_vec4 \
)(vec)

quat quat_rotation(vec3 axis, float angle);
quat conjugate(quat q);
mat4 quat_to_mat4(quat q);

vec4 mat4_mul_vec4(const mat4* mat, const vec4* vec);
mat4 mat4_mul_mat4(const mat4* a, const mat4* b);
quat quat_mul_quat(quat a, quat b);

void translate(mat4* mat, vec3 vec);
void rotate(mat4* mat, quat q);
void scale(mat4* mat, vec3 vec);

enum ProjectionMode {
    PROJECTION_PRESERVE_ASPECT,
    PROJECTION_ADAPT_WIDTH,
    PROJECTION_ADAPT_HEIGHT,
    PROJECTION_UNIFORM_SCALE_INSIDE,
    PROJECTION_UNIFORM_SCALE_OUTSIDE
};

mat4 ortho_projection(enum ProjectionMode mode, float aspect, float near, float far);
mat4 perspective_projection(enum ProjectionMode mode, float aspect, float near, float far, float fov);