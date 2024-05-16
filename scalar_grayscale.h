#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "clamp.h"

typedef struct {
    size_t width;
    size_t height;
    uint8_t *pixels;
} Image;

float approximate_sqrt(float value);



void brightness_contrast(
        const uint8_t *pixels, size_t width, size_t height,
        float a, float b, float c,
        int16_t brightness, float contrast,
        uint8_t *result);

#endif
