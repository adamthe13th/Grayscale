#include "image_io.h"

Image read_ppm(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }


    char buffer[16];
    if (!fgets(buffer, sizeof(buffer), file)) {
        perror("Reading PPM failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }


    if (buffer[0] != 'P' || buffer[1] != '6') {
        fprintf(stderr, "Invalid image format (must be 'P6')\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    
    Image img;
   
    char line[70];

    while (fgets(line, sizeof(line), file) != NULL) {
        // Check if the line starts with # skip if it is
        if (line[0] == '#') {
            continue;
        }

        // Try to parse two integers from the line
        if (sscanf(line, "%zu %zu", &img.width, &img.height) != 2) {
        fprintf(stderr, "Invalid image size (error loading PPM)\n");
        fclose(file);
        exit(EXIT_FAILURE);
        }
        else {break;}
    }

    
    int maxval;
    if (fscanf(file, "%d", &maxval) != 1 || maxval != 255) {
        fprintf(stderr, "Invalid max color value\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    while (fgetc(file) != '\n'); // Skip to the end of the header


    img.pixels = (uint8_t *) malloc(img.width * img.height * 3);
    if (!img.pixels) {
        fprintf(stderr, "Unable to allocate memory\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }


    if (fread(img.pixels, 3 * img.width, img.height, file) != img.height) {
        fprintf(stderr, "Error loading image data\n");
        free(img.pixels);
        fclose(file);
        exit(EXIT_FAILURE);
    }


    fclose(file);
    return img;
}

void write_pgm(const char *filename, Image *img) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    // Write the PGM header: P5 format, width, height, and the maximum value for grayscale
    fprintf(file, "P5\n%zu %zu\n255\n", img->width, img->height);

    // Calculate the size of pixel data for grayscale: width * height
    size_t pixelDataSize = img->width * img->height;

    // Write the grayscale pixel data. Assuming img->pixels already contains grayscale data,
    // where each pixel is represented by a single byte of grayscale value.
    if (fwrite(img->pixels, 1, pixelDataSize, file) != pixelDataSize) {
        if (ferror(file)) {
            perror("Error writing image data");
        }
        fclose(file);
        exit(EXIT_FAILURE);
    }
    
    fclose(file);    
    fprintf(stdout, "Grayscale image saved as %s\n", filename);
}


void free_image(Image *img) {
    free(img->pixels);
}
