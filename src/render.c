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
// TODO better vertex shader
void vertex_shader(const RenderContext* ctx, Vertex* vertex) {
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

        vertex_shader(ctx, &v0);
        vertex_shader(ctx, &v1);
        vertex_shader(ctx, &v2);

        rasterize(ctx, &v0, &v1, &v2);
    }
}