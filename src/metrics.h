#pragma once
#include <stdint.h>

void metrics_mark(uint64_t translation, uint64_t clear, uint64_t render, uint64_t copy, uint64_t present);