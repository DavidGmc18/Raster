#include "render.h"
#include <assert.h>
#include "rasterizer.h"

void clearColorBuffer(const RenderContext* ctx, vec4 clear_color) {
    assert(ctx && "Render context is NULL!");
    for (int y = 0; y < ctx->h; y++) {
        for (int x = 0; x < ctx->w; x++) {
            ctx->pixels[y * ctx->pitch + x] = clear_color;
        }
    }
}

void clearDepthBuffer(const RenderContext* ctx, float clear_value) {
    assert(ctx && "Render context is NULL!");
    for (int y = 0; y < ctx->h; y++) {
        for (int x = 0; x < ctx->w; x++) {
            ctx->depth_buffer[y * ctx->pitch + x] = clear_value;
        }
    }
}

// TODO move out of this file
vec4 vec4_mul_mat4(const vec4* vec, const mat4* mat) {
    vec4 out = {0.0f, 0.0f, 0.0f, 0.0f};
    for (int m = 0; m < 4; m++) {
        for (int k = 0; k < 4; k++) {
            out.v[m] += vec->v[k] * mat->v[k].v[m];
        }
    }
    return out;
}

// TODO move out of this file
// TODO better vertex shader
void vertex_shader(const RenderContext* ctx, const Object* obj, Vertex* vertex) {
    vertex->position = vec4_mul_mat4(&vertex->position, &obj->model);

    vertex->position.x =  ((0.5f * vertex->position.x) + 0.5f) * ctx->w;
    vertex->position.y = ((-0.5f * vertex->position.y) + 0.5f) * ctx->h;
}

#include <stdio.h>

void render(const RenderContext* ctx, const Object* obj) {
    assert(ctx && "Render context is NULL!");
    assert(obj && "Object is NULL!");
    assert(obj->count % 3 == 0 && "Index count must be divisible by 3!");

    for (unsigned int i = 0; i < obj->count; i += 3) {
        unsigned int i0 = obj->indices[i+0];
        unsigned int i1 = obj->indices[i+1];
        unsigned int i2 = obj->indices[i+2];

        Vertex v0 = obj->vertices[i0];
        Vertex v1 = obj->vertices[i1];
        Vertex v2 = obj->vertices[i2];

        vertex_shader(ctx, obj, &v0);
        vertex_shader(ctx, obj, &v1);
        vertex_shader(ctx, obj, &v2);

        rasterize(ctx, &v0, &v1, &v2);
    }
}