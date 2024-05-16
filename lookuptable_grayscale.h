#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "clamp.h"



// For our default values of a, b and c, we have the following lookup table.
extern float lookuptable_R [256];
extern float lookuptable_G [256];
extern float lookuptable_B [256];

void brightness_contrast_V4(
        const uint8_t *pixels, size_t width, size_t height,
        float a, float b, float c,
        int16_t brightness, float contrast,
        uint8_t *result);
