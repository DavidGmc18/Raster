#include "metrics.h"
#include <stdio.h>

#define TIME_FRAMES 256

static float translation[TIME_FRAMES];
static float clear[TIME_FRAMES];
static float render[TIME_FRAMES];
static float copy[TIME_FRAMES];
static float present[TIME_FRAMES];

static int i = 0;

void metrics_mark(uint64_t in_translation, uint64_t in_clear, uint64_t in_render, uint64_t in_copy, uint64_t in_present) {
    translation[i] = (float)in_translation / 1000000.0f;
    clear[i] = (float)in_clear / 1000000.0f;
    render[i] = (float)in_render / 1000000.0f;
    copy[i] = (float)in_copy / 1000000.0f;
    present[i] = (float)in_present / 1000000.0f;

    i++;

    if (i >= TIME_FRAMES) {
        i = 0;
        
        float avg_translation = 0.0f;
        for (int i = 0; i < TIME_FRAMES; i++) avg_translation += translation[i];
        avg_translation /= TIME_FRAMES;

        float avg_clear = 0.0f;
        for (int i = 0; i < TIME_FRAMES; i++) avg_clear += clear[i];
        avg_clear /= TIME_FRAMES;

        float avg_render = 0.0f;
        for (int i = 0; i < TIME_FRAMES; i++) avg_render += render[i];
        avg_render /= TIME_FRAMES;

        float avg_copy = 0.0f;
        for (int i = 0; i < TIME_FRAMES; i++) avg_copy += copy[i];
        avg_copy /= TIME_FRAMES;

        float avg_present = 0.0f;
        for (int i = 0; i < TIME_FRAMES; i++) avg_present += present[i];
        avg_present /= TIME_FRAMES;

        float total = avg_translation + avg_clear + avg_render + avg_copy + avg_present;
        float frame_rate = 1000.0f / total;

        printf(
            "Translation-Clear-Render-Copy-Present times:  %.2f ms  %.2f ms  %.2f ms  %.2f ms  %.2f ms    Total:  %.2f ms    FPS:  %.2f\n",
            avg_translation, avg_clear, avg_render, avg_copy, avg_present, total, frame_rate
        );
    }
}