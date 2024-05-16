#ifndef GRA_PROJECT_THREADED_GRAYSCALE_H
#define GRA_PROJECT_THREADED_GRAYSCALE_H

#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include "scalar_grayscale.h"
#include "clamp.h"


void brightness_contrast_V3(
        const uint8_t *img, size_t width, size_t height,
        float a, float b, float c,
        int16_t brightness, float contrast,
        uint8_t *result,int noT);

#endif
