#ifndef GRA_PROJECT_IMAGE_IO_H
#define GRA_PROJECT_IMAGE_IO_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "scalar_grayscale.h"


Image read_ppm(const char* filename);
void write_pgm(const char* filename, Image* img);
void free_image(Image* img);

#endif
