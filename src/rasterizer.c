#include "rasterizer.h"
#include <stdbool.h>

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
static inline const vec4* fragment_shader(const RenderContext* ctx, vec4* color) {
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

    float z_row = (wa_row * A->position.z + wb_row * B->position.z + wc_row * C->position.z) * inverse_area;
    float z_dy = (-bc.x * A->position.z + -ca.x * B->position.z + -ab.x * C->position.z) * inverse_area;
    float z_dx = (bc.y * A->position.z + ca.y * B->position.z + ab.y * C->position.z) * inverse_area;

    vec4 color_row = {
        .r = (wa_row * A->color.r + wb_row * B->color.r + wc_row * C->color.r) * inverse_area,
        .g = (wa_row * A->color.g + wb_row * B->color.g + wc_row * C->color.g) * inverse_area,
        .b = (wa_row * A->color.b + wb_row * B->color.b + wc_row * C->color.b) * inverse_area,
        .a = (wa_row * A->color.a + wb_row * B->color.a + wc_row * C->color.a) * inverse_area
    };
    vec4 color_dy = {
        .r = (-bc.x * A->color.r + -ca.x * B->color.r + -ab.x * C->color.r) * inverse_area,
        .g = (-bc.x * A->color.g + -ca.x * B->color.g + -ab.x * C->color.g) * inverse_area,
        .b = (-bc.x * A->color.b + -ca.x * B->color.b + -ab.x * C->color.b) * inverse_area,
        .a = (-bc.x * A->color.a + -ca.x * B->color.a + -ab.x * C->color.a) * inverse_area
    };
    vec4 color_dx = {
        .r = (bc.y * A->color.r + ca.y * B->color.r + ab.y * C->color.r) * inverse_area,
        .g = (bc.y * A->color.g + ca.y * B->color.g + ab.y * C->color.g) * inverse_area,
        .b = (bc.y * A->color.b + ca.y * B->color.b + ab.y * C->color.b) * inverse_area,
        .a = (bc.y * A->color.a + ca.y * B->color.a + ab.y * C->color.a) * inverse_area
    };

    for (int y = min_y; y < max_y; y++) {
        int i = y * ctx->pitch + min_x;

        float wa = wa_row;
        float wb = wb_row;
        float wc = wc_row;

        float z = z_row;

        vec4 color = color_row;

        bool flag = false;
        for (int x = min_x; x < max_x; x++) {
            if (wa >= 0.0f && wb >= 0.0f && wc >= 0.0f) {
                if (!flag && x != min_x) {
                    int d = x - min_x;
                    z += d * z_dx;

                    color.r += d * color_dx.r;
                    color.g += d * color_dx.g;
                    color.b += d * color_dx.b;
                    color.a += d * color_dx.a;
                }

                if (z <= ctx->depth_buffer[i]) {
                    ctx->pixels[i] = *fragment_shader(ctx, &color);
                    ctx->depth_buffer[i] = z;
                }

                flag = true;
            } else if (flag) break;

            i++;

            wa += bc.y;
            wb += ca.y;
            wc += ab.y;

            if (flag) {
                z += z_dx;

                color.r += color_dx.r;
                color.g += color_dx.g;
                color.b += color_dx.b;
                color.a += color_dx.a;
            }
        }

        wa_row -= bc.x;
        wb_row -= ca.x;
        wc_row -= ab.x;

        z_row += z_dy;

        color_row.r += color_dy.r;
        color_row.g += color_dy.g;
        color_row.b += color_dy.b;
        color_row.a += color_dy.a;
    }
}