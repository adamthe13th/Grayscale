//This is the SIMD version of the Grayscale algorithm
#include "simd_grayscale.h"

//https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction
float simd_array_addition(__m128 array[], size_t length) {
    __m128 sum = _mm_setzero_ps();

    for (size_t i = 0; i < length; ++i) {
        sum = _mm_add_ps(sum, array[i]);
    }
    __m128 shuf = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 sums = _mm_add_ps(sum, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    return _mm_cvtss_f32(sums);
}

void brightness_contrast_V1(
        const uint8_t *img, size_t width, size_t height,
        float a, float b, float c,
        int16_t brightness, float contrast,
        uint8_t *result) {

    size_t pixelCount = width * height;
    float new_a = a/(a+b+c);
    float new_b = b/(a+b+c);
    float new_c = c/(a+b+c);
    float mean_brightened = 0.0f;
    float variance_afterBrightness = 0.0f;
    float float_brightness = (float) brightness;
    const uint8_t *pixel;

    __m128 a_vec = _mm_load1_ps(&new_a);
    __m128 b_vec = _mm_load1_ps(&new_b);
    __m128 c_vec = _mm_load1_ps(&new_c);
    __m128 brightness_vec = _mm_load1_ps(&float_brightness);

    __m128 *array_for_q_primes = (__m128 *) malloc((pixelCount / 4) * sizeof(__m128));
    if (array_for_q_primes == NULL) {
        fprintf(stderr, "Failed to allocate memory for the storage of Q Primes\n");
        return;
    }
    __m128 *variances = (__m128 *) malloc((pixelCount / 4) * sizeof(__m128));
    if (variances == NULL) {
        fprintf(stderr, "Failed to allocate memory for the storage of Q Primes\n");
        return;
    }
    __m128 *array_for_q_double_primes = (__m128 *) malloc((pixelCount / 4) * sizeof(__m128));
    if (array_for_q_double_primes == NULL) {
        fprintf(stderr, "Failed to allocate memory for the storage of Q Primes\n");
        return;
    }

    for (int i = 0; i < pixelCount / 4; ++i) {
        pixel = &img[i * 12];
        __m128 red = _mm_set_ps(pixel[9], pixel[6], pixel[3], pixel[0]);
        __m128 green = _mm_set_ps(pixel[10], pixel[7], pixel[4], pixel[1]);
        __m128 blue = _mm_set_ps(pixel[11], pixel[8], pixel[5], pixel[2]);
        __m128 D = _mm_add_ps(_mm_add_ps(_mm_mul_ps(a_vec, red), _mm_mul_ps(b_vec, green)), _mm_mul_ps(c_vec, blue));
        __m128 Q_prime = _mm_add_ps(D, brightness_vec);
        Q_prime = _mm_min_ps(_mm_max_ps(Q_prime, _mm_setzero_ps()), _mm_set_ps1(255.0f));
        array_for_q_primes[i] = Q_prime;
    }

    mean_brightened = simd_array_addition(array_for_q_primes, pixelCount / 4);

    for (size_t i = 0; i < pixelCount % 4; ++i) {
        pixel = &img[i * 3];
        float D = (a * pixel[0] + b * pixel[1] + c * pixel[2]) / (a+b+c);
        int Q_prime = CLAMP((int) D + brightness);
        result[i + pixelCount/4] = Q_prime;
        mean_brightened += Q_prime;
    }
    mean_brightened /= pixelCount;
    __m128 mean_brightened_vec = _mm_load1_ps(&mean_brightened);

    // Calculate variance and adjust contrast
    for (size_t i = 0; i < pixelCount / 4; ++i) {
        variances[i] = _mm_sub_ps(array_for_q_primes[i], mean_brightened_vec);
        variances[i] = _mm_mul_ps(variances[i], variances[i]);
    }
    float sumOfVariances = 0.0f;
    for (size_t i = 0; i < pixelCount % 4; ++i) {
        sumOfVariances = (result[i + pixelCount/4] - mean_brightened) * (result[i + pixelCount/4] - mean_brightened);
        variance_afterBrightness += sumOfVariances;
    }

    float std_dev = (simd_array_addition(variances, pixelCount / 4) + variance_afterBrightness);
    std_dev = sqrtf(std_dev / pixelCount);
    float contrast_factor = ((std_dev == 0) ? 0 : contrast / std_dev);
    float contrast_second_term = (1 - contrast_factor) * mean_brightened;

    __m128 contrast_factor_vec = _mm_load1_ps(&contrast_factor);
    __m128 contrast_second_term_vec = _mm_load1_ps(&contrast_second_term);

    // Apply contrast adjustment
    for (size_t i = 0; i < pixelCount / 4; ++i) {
        array_for_q_double_primes[i] = _mm_add_ps(_mm_mul_ps(array_for_q_primes[i], contrast_factor_vec),
                                                  contrast_second_term_vec);
        array_for_q_double_primes[i] = _mm_max_ps(array_for_q_double_primes[i], _mm_setzero_ps());
        array_for_q_double_primes[i] = _mm_min_ps(array_for_q_double_primes[i], _mm_set_ps1(255));
    }
    for (size_t i = 0; i < pixelCount % 4; ++i) {
        float Q_double_prime = (contrast_factor * result[i + pixelCount/4]) + ((1 - contrast_factor) * mean_brightened);
        // Clamp the result and store it back in the result array
        uint8_t Q_double_adjusted = CLAMP((int) Q_double_prime);
        result[i + pixelCount/4] = Q_double_adjusted;
    }


    for (size_t i = 0; i < pixelCount / 4; ++i) {
        __m128i intqdp = _mm_cvttps_epi32(array_for_q_double_primes[i]);
        int elements[4];
        _mm_store_si128((__m128i *) elements, intqdp);

        // Convert each float to uint8_t
        for (int j = 0; j < 4; ++j) {
            // Store the value in the result array
            result[i * 4 + j] = (uint8_t) elements[j];
        }
    }

    free(array_for_q_primes);
    free(variances);
    free(array_for_q_double_primes);
}
