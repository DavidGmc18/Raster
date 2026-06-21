#include "copy.h"
#include <math.h>
#if defined(__AVX2__)
#include <immintrin.h>

__attribute__((always_inline)) static inline void copy8(const Pixel* src, uint8_t* dst) {
    __m256 f32_01 = _mm256_loadu_ps((float*)&src[0]);
    __m256 f32_23 = _mm256_loadu_ps((float*)&src[2]);
    __m256 f32_45 = _mm256_loadu_ps((float*)&src[4]);
    __m256 f32_67 = _mm256_loadu_ps((float*)&src[6]);

    // Gamma correction
    f32_01 = _mm256_sqrt_ps(f32_01);
    f32_23 = _mm256_sqrt_ps(f32_23);
    f32_45 = _mm256_sqrt_ps(f32_45);
    f32_67 = _mm256_sqrt_ps(f32_67);

    const __m256 max = _mm256_set1_ps(255.0f);

    f32_01 = _mm256_mul_ps(f32_01, max);
    f32_23 = _mm256_mul_ps(f32_23, max);
    f32_45 = _mm256_mul_ps(f32_45, max);
    f32_67 = _mm256_mul_ps(f32_67, max);

    __m256i i32_01 = _mm256_cvtps_epi32(f32_01);
    __m256i i32_23 = _mm256_cvtps_epi32(f32_23);
    __m256i i32_45 = _mm256_cvtps_epi32(f32_45);
    __m256i i32_67 = _mm256_cvtps_epi32(f32_67);

    __m256i u16_0123 = _mm256_packus_epi32(i32_01, i32_23);
    __m256i u16_4567 = _mm256_packus_epi32(i32_45, i32_67);

    __m256i u8_01234567 = _mm256_packus_epi16(u16_0123, u16_4567);

    const __m256i perm = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);
    u8_01234567 = _mm256_permutevar8x32_epi32(u8_01234567, perm);
    _mm256_storeu_si256((__m256i*)dst, u8_01234567);
}

__attribute__((always_inline)) static inline void copy1(const Pixel* src, uint8_t* dst) {
    float r = src->r;
    float g = src->g;
    float b = src->b;
    float a = src->a;

    r = (r < 0.0f) ? 0.0f : (r > 255.0f) ? 255.0f : r;
    g = (g < 0.0f) ? 0.0f : (g > 255.0f) ? 255.0f : g;
    b = (b < 0.0f) ? 0.0f : (b > 255.0f) ? 255.0f : b;
    a = (a < 0.0f) ? 0.0f : (a > 255.0f) ? 255.0f : a;

    dst[0] = (uint8_t)(sqrtf(r) * 255.0f);
    dst[1] = (uint8_t)(sqrtf(g) * 255.0f);
    dst[2] = (uint8_t)(sqrtf(b) * 255.0f);
    dst[3] = (uint8_t)(sqrtf(a) * 255.0f);
}

void copy(const RenderContext* ctx, SDL_Surface* surface) {
    struct { uint8_t r, g, b, a; }* p = surface->pixels;

    for (int y = 0; y < ctx->h; y++) {
        int j = y * ctx->pitch;

        int x = 0;
        while (x < (ctx->w - 7)) {
            int i = j + x;
            copy8(&ctx->pixels[i], (uint8_t*)&p[i]);
            x += 8;
        }

        while (x < ctx->w) {
            int i = j + x;
            copy1(&ctx->pixels[i], (uint8_t*)&p[i]);
            x++;
        }
    }
}

#else

void copy(const RenderContext* ctx, SDL_Surface* surface) {
    struct { uint8_t r, g, b, a; }* p = surface->pixels;

    for (int y = 0; y < ctx->h; y++) {
        int i = y * ctx->pitch;
        for (int x = 0; x < ctx->w; x++) {
            Pixel pixel = ctx->pixels[i];
            
            p[i].r = (uint8_t)(sqrtf(pixel.r) * 255.0f);
            p[i].g = (uint8_t)(sqrtf(pixel.g) * 255.0f);
            p[i].b = (uint8_t)(sqrtf(pixel.b) * 255.0f);
            p[i].a = (uint8_t)(sqrtf(pixel.a) * 255.0f);

            i++;
        }
    }
}

#endif