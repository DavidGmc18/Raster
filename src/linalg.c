#include "linalg.h"

vec4 mat4_mul_vec4(const mat4* mat, const vec4* vec) {
    vec4 out = {0.0f, 0.0f, 0.0f, 0.0f};
    for (int m = 0; m < 4; m++) {
        for (int k = 0; k < 4; k++) {
            out.v[m] += mat->v[m].v[k] * vec->v[k];
        }
    }
    return out;
}