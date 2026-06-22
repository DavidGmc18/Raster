#include "linalg.h"
#include <math.h>

inline float _length_vec2(vec2 vec) {
    return sqrtf((vec.x * vec.x) + (vec.y * vec.y));
}

inline float _length_vec3(vec3 vec) {
    return sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}

inline float _length_vec4(vec4 vec) {
    return sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) + (vec.w * vec.w));
}


inline vec2 _normalize_vec2(vec2 vec) {
    float len = _length_vec2(vec);
    float inv = (len == 0.0f) ? 0.0f : (1.0f / len);
    vec.x *= inv;
    vec.y *= inv;
    return vec;
}

inline vec3 _normalize_vec3(vec3 vec) {
    float len = _length_vec3(vec);
    float inv = (len == 0.0f) ? 0.0f : (1.0f / len);
    vec.x *= inv;
    vec.y *= inv;
    vec.z *= inv;
    return vec;
}

inline vec4 _normalize_vec4(vec4 vec) {
    float len = _length_vec4(vec);
    float inv = (len == 0.0f) ? 0.0f : (1.0f / len);
    vec.x *= inv;
    vec.y *= inv;
    vec.z *= inv;
    vec.w *= inv;
    return vec;
}


inline quat quat_rotation(vec3 axis, float angle) {
    axis = normalize(axis);
    angle /= 2.0f;
    float sin = sinf(angle);
    return (quat){
        .i = axis.i * sin,
        .j = axis.j * sin,
        .k = axis.k * sin,
        .w = cosf(angle)
    };
}

inline mat4 quat_to_mat4(quat q) {
    float xx = q.i * q.i;
    float yy = q.j * q.j;
    float zz = q.k * q.k;

    float xy = q.i * q.j;
    float xz = q.i * q.k;
    float yz = q.j * q.k;

    float wx = q.w * q.i;
    float wy = q.w * q.j;
    float wz = q.w * q.k;

    return mat4(
        1.0f - 2.0f * (yy + zz),    2.0f * (xy + wz),           2.0f * (xz - wy),           0.0f,
        2.0f * (xy - wz),           1.0f - 2.0f * (xx + zz),    2.0f * (yz + wx),           0.0f,
        2.0f * (xz + wy),           2.0f * (yz - wx),           1.0f - 2.0f * (xx + yy),    0.0f,
        0.0f,                       0.0f,                       0.0f,                       1.0f
    );
}


inline vec4 mat4_mul_vec4(const mat4* mat, const vec4* vec) {
    vec4 out = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int m = 0; m < 4; m++)
    for (int k = 0; k < 4; k++)
        out.v[m] += mat->v[m].v[k] * vec->v[k];
    return out;
}

inline mat4 mat4_mul_mat4(const mat4* a, const mat4* b) {
    mat4 c = ZERO_MAT4;
    for (int m = 0; m < 4; m++)
    for (int n = 0; n < 4; n++)
    for (int k = 0; k < 4; k++)
        c.v[m].v[n] += a->v[m].v[k] * b->v[k].v[n];
    return c;
}


inline void translate(mat4* mat, vec3 vec) {
    mat->x.w = vec.x;
    mat->y.w = vec.y;
    mat->z.w = vec.z;
}

void rotate(mat4* mat, quat q) {
    mat4 qm = quat_to_mat4(q);
    *mat = mat4_mul_mat4(mat, &qm);
}