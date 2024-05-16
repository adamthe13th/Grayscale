#include "scalar_grayscale.h"

//This is a simple version of the Newton Raphson Algorithm
float approximate_sqrt(float value) {
    double x = value;
    double y = 1.0f;
    float e = 0.000001f; // e decides the accuracy level
    while (x - y > e) {
        x = (x + y) / 2.0f;
        y = value / x;
    }
    return x;
}


void brightness_contrast(
        const uint8_t *img, size_t width, size_t height,
        float a, float b, float c,
        int16_t brightness, float contrast,
        uint8_t *result) {

    size_t pixelCount = width * height;
    float sumOfSquares = 0.0f;
    float mean_brightened = 0.0f;
    float reciprocal_of_sum_of_coeffs = 1 / (a + b + c);
    //calculating relative coefficients for better efficiency
    float ra = a*reciprocal_of_sum_of_coeffs;
    float rb = b*reciprocal_of_sum_of_coeffs;
    float rc = c*reciprocal_of_sum_of_coeffs;
    // Calculate brightness and mean brightness
    for (size_t i = 0; i < pixelCount; ++i) {
        const uint8_t *pixel = &img[i * 3];
        float D = (ra * pixel[0] + rb * pixel[1] + rc * pixel[2]);
        int Q_prime = CLAMP((int) D + brightness);
        result[i] = Q_prime;
        mean_brightened += Q_prime;
    }
    mean_brightened /= pixelCount;
    float variance_afterBrightness = 0.0f;

    for (size_t i = 0; i < pixelCount; ++i) {
        sumOfSquares = (result[i] - mean_brightened) * (result[i] - mean_brightened);
        variance_afterBrightness += sumOfSquares;
    }
    variance_afterBrightness /= pixelCount;
    //printf("\n%f\n",variance_afterBrightness);
    // now we adjust the contrast using the given variable
    float std_dev = approximate_sqrt(variance_afterBrightness);
    float contrast_factor = ((std_dev == 0) ? 0 : contrast / std_dev);
    //printf("\n%f",contrast_factor);
    // Apply contrast adjustment to each pixel
    for (size_t i = 0; i < pixelCount; ++i) {
        float Q_double_prime = (contrast_factor * result[i]) + ((1 - contrast_factor) * mean_brightened);
        result[i] = CLAMP((int) Q_double_prime);
    }

}


