#ifndef TEST_GRAYSCALE_H
#define TEST_GRAYSCALE_H

#include <stddef.h>  // For size_t
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "image_io.h"
#include <math.h>
#include "desaturation.h"
#include "simd_grayscale.h"
// Function to check if one color channel is dominant in a PPM image.
int check_color_channel_dominance(const char *filename, int dominant_channel_index, float dominant_coeff);

// Runs a test for specific coefficients and checks the resulting image for color channel dominance.
void run_coefficients_test(const char *description, const char *coeffs, const char *input_file, const char *output_file, int dominant_channel_index, float dominant_coeff);

// Executes a command and prints its execution status.
void run_command(const char *command);

// Runs all test cases.
//void runTestCases();
void runAllTests();

#endif /* TEST_GRAYSCALE_H */
