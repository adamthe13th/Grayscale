#pragma once

#include <xmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "clamp.h"



float simd_array_addition(__m128 array[], size_t length);

void brightness_contrast_V1(
        const uint8_t *img, size_t width, size_t height,
        float a, float b, float c,
        int16_t brightness, float contrast,
        uint8_t *result);
