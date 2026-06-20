#pragma once

typedef union vec2_t {
    float v[2];
    struct { float x, y; };
    struct { float i, j; };
} vec2;

#define vec2(a, b) ((vec2){.v = {a, b}})

typedef union vec3_t {
    float v[3];
    struct { float x, y, z; };
    struct { float i, j, k; };
    struct { float r, g, b; };
} vec3;

#define vec3(a, b, c) ((vec3){.v = {a, b, c}})

typedef union vec4_t {
    float v[4];
    struct { float x, y, z, w; };
    struct { float i, j, k, l; };
    struct { float r, g, b, a; };
} vec4;

#define _vec4_dispatch(_1, _2, _3, _4, MACRO, ...) MACRO
#define _vec4_3f(a, b, c)    ((vec4){.v = {a, b, c, 1.0f}})
#define _vec4_4f(a, b, c, d) ((vec4){.v = {a, b, c, d}})
#define vec4(...) _vec4_dispatch(__VA_ARGS__, _vec4_4f, _vec4_3f)(__VA_ARGS__)



typedef vec4 Pixel;
typedef struct pixel_buffer_t {
    int w, h;
    int pitch;
    Pixel* pixels;
    float* depth_buffer;
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
} Object;