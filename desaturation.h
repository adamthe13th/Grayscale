#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "clamp.h"
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

void brightness_contrast_V2(
    const uint8_t* pixels, size_t width, size_t height,
    float a, float b, float c,
    int16_t brightness, float contrast,
    uint8_t* result);