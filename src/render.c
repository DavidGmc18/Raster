#include "render.h"
#include <assert.h>
#include "rasterizer.h"
#include "linalg.h"
#include <stdbool.h>

#if defined(__AVX2__)
#include <immintrin.h>

void clearColorBuffer(const RenderContext* ctx, vec4 clear_color) {
    assert(ctx && "Render context is NULL!");

    __m256 color = _mm256_broadcast_ps((const __m128*)&clear_color);

    for (int y = 0; y < ctx->h; y++) {
        int j = y * ctx->pitch;
        int x = 0;
        
        while (x < (ctx->w - 3)) {
            _mm256_stream_ps((float*)&ctx->pixels[j + x + 0], color);
            _mm256_stream_ps((float*)&ctx->pixels[j + x + 2], color);
            x += 4;
        }

        while (x < ctx->w) {
            ctx->pixels[j + x++] = clear_color;
        }
    }

    _mm_sfence();
}

void clearDepthBuffer(const RenderContext* ctx, float clear_value) {
    assert(ctx && "Render context is NULL!");

    __m256 value = _mm256_set1_ps(clear_value);

    for (int y = 0; y < ctx->h; y++) {
        int j = y * ctx->pitch;
        int x = 0;

        while (x < (ctx->w - 7)) {
            _mm256_stream_ps((float*)&ctx->depth_buffer[j + x], value);
            x += 8;
        }

        while (x < ctx->w) {
            ctx->depth_buffer[j + x++] = clear_value;
        }
    }

    _mm_sfence();
}

#else

void clearColorBuffer(const RenderContext* ctx, vec4 clear_color) {
    assert(ctx && "Render context is NULL!");
    for (int y = 0; y < ctx->h; y++) {
        int i = y * ctx->pitch;
        for (int x = 0; x < ctx->w; x++) {
            ctx->pixels[i++] = clear_color;
        }
    }
}

void clearDepthBuffer(const RenderContext* ctx, float clear_value) {
    assert(ctx && "Render context is NULL!");
    for (int y = 0; y < ctx->h; y++) {
        int j = y * ctx->pitch;

        for (int x = 0; x < ctx->w; x++) {
            ctx->depth_buffer[j + x] = clear_value;
        }
    }
}

#endif

// TODO move out of this file
// TODO better vertex shader
inline static void vertex_shader(const RenderContext* ctx, const Object* obj, Vertex* vertex) {
    vertex->position = mat4_mul_vec4(&obj->model, &vertex->position);
    vertex->position = mat4_mul_vec4(&ctx->view, &vertex->position);
    vertex->position = mat4_mul_vec4(&ctx->projection, &vertex->position);
}

inline static void post_clip_transform(const RenderContext* ctx, Vertex* vertex) {
    // Perspective divide
    vertex->position.x /= vertex->position.w;
    vertex->position.y /= vertex->position.w;

    // Convert to NDC
    vertex->position.x = ((vertex->position.x / 2.0f) + 0.5f) * ctx->w;
    vertex->position.y = ((-vertex->position.y / 2.0f) + 0.5f) * ctx->h;
}

#define CLIP(v_in, v_out, near, v_new) do { \
    float _t = ((near) - (v_in).position.w) / ((v_out).position.w - (v_in).position.w); \
    (v_new).position = lerp((v_in).position, (v_out).position, _t); \
    (v_new).color = lerp((v_in).color, (v_out).color, _t); \
} while (0)

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
        Vertex v3;

        vertex_shader(ctx, obj, &v0);
        vertex_shader(ctx, obj, &v1);
        vertex_shader(ctx, obj, &v2);

        // Clip
        float near = -ctx->projection.z.w / ctx->projection.z.z;
        
        bool clip_v0 = v0.position.w < near;
        bool clip_v1 = v1.position.w < near;
        bool clip_v2 = v2.position.w < near;

        bool render_v3 = false;

        if (clip_v0 && clip_v1 && clip_v2) {
            continue;
        } else if (clip_v0 && clip_v1 && !clip_v2) {
            CLIP(v2, v0, near, v0);
            CLIP(v2, v1, near, v1);
        } else if (clip_v0 && !clip_v1 && clip_v2) {
            CLIP(v1, v0, near, v0);
            CLIP(v1, v2, near, v2);
        } else if (!clip_v0 && clip_v1 && clip_v2) {
            CLIP(v0, v1, near, v1);
            CLIP(v0, v2, near, v2);
        } else if (!clip_v0 && !clip_v1 && clip_v2) {
            render_v3 = true;
            CLIP(v0, v2, near, v3);
            CLIP(v1, v2, near, v2);
        } else if (clip_v0 && !clip_v1 && !clip_v2) {
            render_v3 = true;
            CLIP(v2, v0, near, v3);
            CLIP(v1, v0, near, v0);
        } else if (!clip_v0 && clip_v1 && !clip_v2) {
            render_v3 = true;
            v3 = v2;
            CLIP(v2, v1, near, v2);
            CLIP(v0, v1, near, v1);
        }

        // Post clip
        post_clip_transform(ctx, &v0);
        post_clip_transform(ctx, &v1);
        post_clip_transform(ctx, &v2);

        rasterize(ctx, &v0, &v1, &v2);

        if (render_v3) {
            post_clip_transform(ctx, &v3);
            rasterize(ctx, &v0, &v2, &v3);
        }
    }
}