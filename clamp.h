#pragma once
#include <stdint.h>

#define CLAMP(value) ((value < 0) ? 0 : ((value > 255) ? 255 : (uint8_t)(value)))