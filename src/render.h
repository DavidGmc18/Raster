#pragma once

#include "types.h"

void clearColorBuffer(const RenderContext* ctx, vec4 clear_color);
void clearDepthBuffer(const RenderContext* ctx, float clear_value);

void render(const RenderContext* ctx, const Object* obj);