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

Vertex cube_vertices[] = {
    Vertex(vec4(-0.5f, -0.5f, -0.5f), vec4(0.0f, 0.6f, 0.0f, 1.0f)),
    Vertex(vec4( 0.5f, -0.5f, -0.5f), vec4(0.0f, 0.6f, 0.0f, 1.0f)),
    Vertex(vec4( 0.5f,  0.5f, -0.5f), vec4(0.0f, 0.6f, 0.0f, 1.0f)),
    Vertex(vec4(-0.5f,  0.5f, -0.5f), vec4(0.0f, 0.6f, 0.0f, 1.0f)),

    Vertex(vec4(-0.5f, -0.5f,  0.5f), vec4(0.7f, 0.3f, 0.0f, 1.0f)),
    Vertex(vec4(-0.5f, -0.5f, -0.5f), vec4(0.7f, 0.3f, 0.0f, 1.0f)),
    Vertex(vec4(-0.5f,  0.5f, -0.5f), vec4(0.7f, 0.3f, 0.0f, 1.0f)),
    Vertex(vec4(-0.5f,  0.5f,  0.5f), vec4(0.7f, 0.3f, 0.0f, 1.0f)),

    Vertex(vec4(-0.5f,  0.5f, -0.5f), vec4(0.6f, 0.6f, 0.6f, 1.0f)),
    Vertex(vec4( 0.5f,  0.5f, -0.5f), vec4(0.6f, 0.6f, 0.6f, 1.0f)),
    Vertex(vec4( 0.5f,  0.5f,  0.5f), vec4(0.6f, 0.6f, 0.6f, 1.0f)),
    Vertex(vec4(-0.5f,  0.5f,  0.5f), vec4(0.6f, 0.6f, 0.6f, 1.0f)),

    Vertex(vec4(-0.5f, -0.5f,  0.5f), vec4(0.5f, 0.5f, 0.0f, 1.0f)),
    Vertex(vec4( 0.5f, -0.5f,  0.5f), vec4(0.5f, 0.5f, 0.0f, 1.0f)),
    Vertex(vec4( 0.5f, -0.5f, -0.5f), vec4(0.5f, 0.5f, 0.0f, 1.0f)),
    Vertex(vec4(-0.5f, -0.5f, -0.5f), vec4(0.5f, 0.5f, 0.0f, 1.0f)),

    Vertex(vec4( 0.5f, -0.5f, -0.5f), vec4(0.5f, 0.0f, 0.0f, 1.0f)),
    Vertex(vec4( 0.5f, -0.5f,  0.5f), vec4(0.5f, 0.0f, 0.0f, 1.0f)),
    Vertex(vec4( 0.5f,  0.5f,  0.5f), vec4(0.5f, 0.0f, 0.0f, 1.0f)),
    Vertex(vec4( 0.5f,  0.5f, -0.5f), vec4(0.5f, 0.0f, 0.0f, 1.0f)),

    Vertex(vec4( 0.5f, -0.5f,  0.5f), vec4(0.0f, 0.0f, 0.5f, 1.0f)),
    Vertex(vec4(-0.5f, -0.5f,  0.5f), vec4(0.0f, 0.0f, 0.5f, 1.0f)),
    Vertex(vec4(-0.5f,  0.5f,  0.5f), vec4(0.0f, 0.0f, 0.5f, 1.0f)),
    Vertex(vec4( 0.5f,  0.5f,  0.5f), vec4(0.0f, 0.0f, 0.5f, 1.0f)),
};

unsigned int cube_indices[] = {
    0,  1,  2,      0,  2,  3,      // front
    4,  5,  6,      4,  6,  7,      // left
    8,  9,  10,     8,  10, 11,     // top
    12, 13, 14,     12, 14, 15,     // down
    16, 17, 18,     16, 18, 19,     // right
    20, 21, 22,     20, 22, 23,     // back
};

Vertex ground_vertices[] = {
    Vertex(vec4(-0.5f,  0.0f, -0.5f), GRAY(0.5f, 1.0f)),
    Vertex(vec4( 0.0f,  0.0f, -0.5f), GRAY(0.5f, 1.0f)),
    Vertex(vec4( 0.0f,  0.0f,  0.0f), GRAY(0.5f, 1.0f)),
    Vertex(vec4(-0.5f,  0.0f,  0.0f), GRAY(0.5f, 1.0f)),

    Vertex(vec4( 0.0f,  0.0f, -0.5f), GRAY(0.1f, 1.0f)),
    Vertex(vec4( 0.5f,  0.0f, -0.5f), GRAY(0.1f, 1.0f)),
    Vertex(vec4( 0.5f,  0.0f,  0.0f), GRAY(0.1f, 1.0f)),
    Vertex(vec4( 0.0f,  0.0f,  0.0f), GRAY(0.1f, 1.0f)),

    Vertex(vec4( 0.0f,  0.0f,  0.0f), GRAY(0.5f, 1.0f)),
    Vertex(vec4( 0.5f,  0.0f,  0.0f), GRAY(0.5f, 1.0f)),
    Vertex(vec4( 0.5f,  0.0f,  0.5f), GRAY(0.5f, 1.0f)),
    Vertex(vec4( 0.0f,  0.0f,  0.5f), GRAY(0.5f, 1.0f)),

    Vertex(vec4(-0.5f,  0.0f,  0.0f), GRAY(0.1f, 1.0f)),
    Vertex(vec4( 0.0f,  0.0f,  0.0f), GRAY(0.1f, 1.0f)),
    Vertex(vec4( 0.0f,  0.0f,  0.5f), GRAY(0.1f, 1.0f)),
    Vertex(vec4(-0.5f,  0.0f,  0.5f), GRAY(0.1f, 1.0f)),
};

unsigned int ground_indices[] = {
    0,  1,  2,      0,  2,  3,
    4,  5,  6,      4,  6,  7,
    8,  9,  10,     8,  10, 11,
    12, 13, 14,     12, 14, 15,
};

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Raster", WIDTH, HEIGHT, 0);
    assert(window && "Failed to create window!");

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    assert(surface && "Failed to get surface!");
    assert((surface->format == SDL_PIXELFORMAT_XRGB8888 || surface->format == SDL_PIXELFORMAT_ARGB8888) && "Current pixel format is not supported!");

    SDL_Surface* canvas = SDL_CreateSurface(WIDTH, HEIGHT, SDL_PIXELFORMAT_RGBA128_FLOAT);
    assert(canvas && "Failed to create canvas!");
    assert(canvas->pitch % sizeof(Pixel) == 0 && "Pitch is not multiple of sizeof(Pixel)");

    RenderContext ctx = {
        .w = canvas->w,
        .h = canvas->h,
        .pitch = canvas->pitch / sizeof(Pixel),
        .pixels = canvas->pixels,
        .depth_buffer = aligned_alloc(32, canvas->h * (canvas->pitch / sizeof(Pixel)) * sizeof(float)),
        .projection = perspective_projection(PROJECTION_UNIFORM_SCALE_INSIDE, (float)WIDTH / (float)HEIGHT, 0.001f, 10.0f, M_PI / 2.0f)
    };
    assert(ctx.depth_buffer && "Failed to malloc depth buffer");

    Object cube = {
        .vertices = cube_vertices,
        .indices = cube_indices,
        .count = sizeof(cube_indices) / sizeof(unsigned int)
    };

    Object ground = {
        .vertices = ground_vertices,
        .indices = ground_indices,
        .count = sizeof(ground_indices) / sizeof(unsigned int)
    };

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        uint64_t A = SDL_GetTicksNS();

        cube.model = IDENTITY_MAT4;
        float t = (float)A / 1000000000.0f;
        rotate(&cube.model, quat_rotation(vec3(1.0f, 1.0f, 1.0f), t));
        translate(&cube.model, vec3(0.0f, 0.0f, 2.0f));

        ground.model = IDENTITY_MAT4;
        scale(&ground.model, vec3(3.0f, 3.0f, 3.0f));
        translate(&ground.model, vec3(0.0f, -1.0f, 2.0f));

        uint64_t B = SDL_GetTicksNS();

        // Clear buffers
        clearColorBuffer(&ctx, vec4(0.0f, 0.0f, 0.0f, 1.0f));
        clearDepthBuffer(&ctx, 1.0f);

        uint64_t C = SDL_GetTicksNS();
        
        // Render
        render(&ctx, &cube);
        render(&ctx, &ground);

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