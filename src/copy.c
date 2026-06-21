#include "copy.h"
#include <stdio.h>

void copy(const RenderContext* ctx, SDL_Surface* surface) {
    struct { uint8_t r, g, b, a; }* p = surface->pixels;

    for (int y = 0; y < ctx->h; y++) {
        int i = y * ctx->pitch;
        for (int x = 0; x < ctx->w; x++) {
            Pixel pixel = ctx->pixels[i];
            
            p[i].r = (uint8_t)(pixel.r * 255.0f);
            p[i].g = (uint8_t)(pixel.g * 255.0f);
            p[i].b = (uint8_t)(pixel.b * 255.0f);
            p[i].a = (uint8_t)(pixel.a * 255.0f);

            i++;
        }
    }
}