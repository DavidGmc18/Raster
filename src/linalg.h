#pragma once

#include "types.h"

vec4 mat4_mul_vec4(const mat4* mat, const vec4* vec);

void translate(mat4* mat, vec3 vec);