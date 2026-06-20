#include "rasterizer.h"

#define UPDATE_BOUNDS(vertex) { \
    int x = (int)((vertex)->position.x); \
    if (x < min_x) min_x = x; \
    if (x > max_x) max_x = x; \
    int y = (int)((vertex)->position.y); \
    if (y < min_y) min_y = y; \
    if (y > max_y) max_y = y; \
}

// TODO move out of this file
// TODO better fragment shader
// TODO depth buffer
const vec4* fragment_shader(const RenderContext* ctx, vec4* color) {
    return color;
}

void rasterize(const RenderContext* ctx, const Vertex* A, const Vertex* B, const Vertex* C) {
    int min_x = ctx->w;
    int max_x = 0;
    int min_y = ctx->h;
    int max_y = 0;

    UPDATE_BOUNDS(A);
    UPDATE_BOUNDS(B);
    UPDATE_BOUNDS(C);

    if (min_x < 0) min_x = 0;
    if (max_x > ctx->w) max_x = ctx->w;
    if (min_y < 0) min_y = 0;
    if (max_y > ctx->h) max_y = ctx->h;

    vec2 ab = {B->position.x - A->position.x, B->position.y - A->position.y};
    vec2 bc = {C->position.x - B->position.x, C->position.y - B->position.y};
    vec2 ca = {A->position.x - C->position.x, A->position.y - C->position.y};

    vec2 ap = {min_x - A->position.x, min_y - A->position.y};
    vec2 bp = {min_x - B->position.x, min_y - B->position.y};
    vec2 cp = {min_x - C->position.x, min_y - C->position.y};

    float wa_row = (bp.x * bc.y) - (bp.y * bc.x);
    float wb_row = (cp.x * ca.y) - (cp.y * ca.x);
    float wc_row = (ap.x * ab.y) - (ap.y * ab.x);

    float total_area = wa_row + wb_row + wc_row;
    if (total_area <= 0.0f) return;

    float inverse_area = 1.0f / total_area;

    for (int y = min_y; y < max_y; y++) {
        int j = y * ctx->pitch;

        float wa = wa_row;
        float wb = wb_row;
        float wc = wc_row;

        for (int x = min_x; x < max_x; x++) {
            if (wa >= 0.0f && wb >= 0.0f && wc >= 0.0f) {
                float alpha = wa * inverse_area;
                float beta = wb * inverse_area;
                float gamma = wc * inverse_area;

                float z = alpha * A->position.z + beta * B->position.z + gamma * C->position.z;

                vec4 color = {
                    .r = alpha * A->color.r + beta * B->color.r + gamma * C->color.r,
                    .g = alpha * A->color.g + beta * B->color.g + gamma * C->color.g,
                    .b = alpha * A->color.b + beta * B->color.b + gamma * C->color.b,
                    .a = alpha * A->color.a + beta * B->color.a + gamma * C->color.a
                };

                if (z > ctx->depth_buffer[j + x]) continue;

                ctx->pixels[j + x] = *fragment_shader(ctx, &color);
                ctx->depth_buffer[j + x] = z;
            }

            wa += bc.y;
            wb += ca.y;
            wc += ab.y;
        }

        wa_row -= bc.x;
        wb_row -= ca.x;
        wc_row -= ab.x;
    }
}