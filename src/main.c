#include <SDL3/SDL.h>
#include "types.h"
#include "render.h"
#include <assert.h>
#include <stdlib.h>
#include "linalg.h"
#include <math.h>
#include "copy.h"
#include "metrics.h"

const int WIDTH = 1280; // must be multiple of 8
const int HEIGHT = 720;

// Vertex vertices[8] = {
//     Vertex(vec4(-0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 1.0f, 1.0f)),
//     Vertex(vec4(-0.5f, -0.5f,  0.5f), vec4(1.0f, 1.0f, 1.0f, 1.0f)),
//     Vertex(vec4(-0.5f,  0.5f, -0.5f), vec4(1.0f, 1.0f, 1.0f, 1.0f)),
//     Vertex(vec4(-0.5f,  0.5f,  0.5f), vec4(1.0f, 1.0f, 1.0f, 1.0f)),
//     Vertex(vec4( 0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 1.0f, 1.0f)),
//     Vertex(vec4( 0.5f, -0.5f,  0.5f), vec4(1.0f, 1.0f, 1.0f, 1.0f)),
//     Vertex(vec4( 0.5f,  0.5f, -0.5f), vec4(1.0f, 1.0f, 1.0f, 1.0f)),
//     Vertex(vec4( 0.5f,  0.5f,  0.5f), vec4(1.0f, 1.0f, 1.0f, 1.0f)),
// };

// unsigned int indices[36] = {
//     0, 1, 3,    0, 3, 2,
//     4, 6, 7,    4, 7, 5,
//     0, 4, 5,    0, 5, 1,
//     2, 3, 7,    2, 7, 6,
//     0, 2, 6,    0, 6, 4,
//     1, 5, 7,    1, 7, 3
// };

Vertex vertices[3] = {
    Vertex(vec4( 0.0f,  0.5f,  0.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f)),
    Vertex(vec4(-0.5f, -0.5f,  0.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f)),
    Vertex(vec4( 0.5f, -0.5f,  0.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f))
};

unsigned int indices[3] = {0, 1, 2};

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Raster", WIDTH, HEIGHT, 0);
    assert(window && "Failed to create window!");

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    assert(surface && "Failed to get surface!");

    SDL_Surface* canvas = SDL_CreateSurface(WIDTH, HEIGHT, SDL_PIXELFORMAT_RGBA128_FLOAT);
    assert(canvas && "Failed to create canvas!");
    assert(canvas->pitch % sizeof(Pixel) == 0 && "Pitch is not multiple of sizeof(Pixel)");

    RenderContext ctx = {
        .w = canvas->w,
        .h = canvas->h,
        .pitch = canvas->pitch / sizeof(Pixel),
        .pixels = canvas->pixels,
        .depth_buffer = aligned_alloc(32, canvas->h * (canvas->pitch / sizeof(Pixel)) * sizeof(float))
    };
    assert(ctx.depth_buffer && "Failed to malloc depth buffer");

    // Object cube = {
    //     .vertices = vertices,
    //     .indices = indices,
    //     .count = 36
    // };

    Object triangle = {
        .vertices = vertices,
        .indices = indices,
        .count = 3,
        .model = IDENTITY_MAT4
    };

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        uint64_t A = SDL_GetTicksNS();

        triangle.model = IDENTITY_MAT4;
        float t = (float)A / 1000000000.0f;
        float r = 0.4f;
        translate(&triangle.model, vec3(r * cos(t), r * sin(t), 0.0f));
        rotate(&triangle.model, quat_rotation(vec3(0.0f, 0.0f, 1.0f), t));

        uint64_t B = SDL_GetTicksNS();

        // Clear buffers
        clearColorBuffer(&ctx, vec4(0.0f, 0.0f, 0.0f, 1.0f));
        clearDepthBuffer(&ctx, 1.0f);

        uint64_t C = SDL_GetTicksNS();
        
        // Render
        render(&ctx, &triangle);

        uint64_t D = SDL_GetTicksNS();

        // Copy
        copy(&ctx, surface);
    
        uint64_t E = SDL_GetTicksNS();

        // Present
        SDL_UpdateWindowSurface(window);

        uint64_t F = SDL_GetTicksNS();

        metrics_mark(B - A, C - B, D - C, E - D, F - E);
    }

    free(ctx.depth_buffer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}