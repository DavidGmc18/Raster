#pragma once

// vec2
typedef union vec2_t {
    float v[2];
    struct { float x, y; };
    struct { float i, j; };
} vec2;

#define vec2(_v0, _v1) ((vec2){.v = {(_v0), (_v1)}})

// vec3
typedef union vec3_t {
    float v[3];
    struct { float x, y, z; };
    struct { float i, j, k; };
    struct { float r, g, b; };
} vec3;

#define vec3(_v0, _v1, _v2) ((vec3){.v = {(_v0), (_v1), (_v2)}})

// vec4
typedef union vec4_t {
    float v[4];
    struct { float x, y, z, w; };
    struct { float i, j, k, l; };
    struct { float r, g, b, a; };
} vec4;

#define GRAY(rgb, a) (vec4((rgb), (rgb), (rgb), (a)))

#define _vec4_dispatch(_1, _2, _3, _4, MACRO, ...) MACRO
#define _vec4_3f(_v0, _v1, _v2)    ((vec4){.v = {(_v0), (_v1), (_v2), 1.0f}})
#define _vec4_4f(_v0, _v1, _v2, _v3) ((vec4){.v = {(_v0), (_v1), (_v2), (_v3)}})
#define vec4(...) _vec4_dispatch(__VA_ARGS__, _vec4_4f, _vec4_3f)(__VA_ARGS__)

// quat
typedef struct quat_t {
    float i, j, k, w;
} quat;

#define QUAT_IDENTITY (quat){0.0f, 0.0f, 0.0f, 1.0f}

// mat4
typedef union mat4_t {
    float m[16];
    vec4 v[4];
    struct { vec4 x, y, z, w; };
    struct { vec4 i, j, k, l; };
    struct { vec4 a, b, c, d; };
} mat4;

#define mat4(\
    _m00, _m01, _m02, _m03, \
    _m10, _m11, _m12, _m13, \
    _m20, _m21, _m22, _m23, \
    _m30, _m31, _m32, _m33  \
) ((mat4){.m = { \
    (_m00), (_m01), (_m02), (_m03), \
    (_m10), (_m11), (_m12), (_m13), \
    (_m20), (_m21), (_m22), (_m23), \
    (_m30), (_m31), (_m32), (_m33)  \
}})

#define ZERO_MAT4 (mat4){ .m = { \
    0.0f, 0.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, 0.0f, \
}}

#define IDENTITY_MAT4 (mat4){ .m = { \
    1.0f, 0.0f, 0.0f, 0.0f, \
    0.0f, 1.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 1.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, 1.0f, \
}}

// rendering types
typedef vec4 Pixel;
typedef struct pixel_buffer_t {
    int w, h;
    int pitch;
    Pixel* pixels;
    float* depth_buffer;
    mat4 view;
    mat4 projection;
} RenderContext;

typedef struct vertex_t {
    vec4 position;
    vec4 color;
} Vertex;

#define Vertex(position, color) ((Vertex){position, color})

typedef struct object_t {
    Vertex* vertices;
    unsigned int* indices;
    unsigned int count;
    mat4 model;
} Object;