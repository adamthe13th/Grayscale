#include "threaded_grayscale.h"

typedef struct {
    const uint8_t *img;
    uint8_t *result;
    size_t start;
    size_t end;
    float a, b, c;
    int16_t brightness;
    float contrast;
    float mean_brightened;
    float contrast_factor;
} ThreadData;

// Utility function to clamp the pixel values


void* calculate_q_primes(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    for (size_t idx = data->start; idx < data->end; ++idx) {
        const uint8_t *pixel = &data->img[idx * 3];
        float D = (data->a * pixel[0] + data->b * pixel[1] + data->c * pixel[2]);
        int Q_prime = CLAMP((int) (D + data->brightness));
        data->result[idx] = (uint8_t) Q_prime;
    }
    return NULL;
}

void* calculate_q_double_prime(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    for (size_t idx = data->start; idx < data->end; ++idx) {
        float Q_double_prime = (data->contrast_factor * data->result[idx]) + ((1 - data->contrast_factor) * data->mean_brightened);
        data->result[idx] = CLAMP((int) Q_double_prime);
    }
    return NULL;
}

void brightness_contrast_V3(
        const uint8_t *img, size_t width, size_t height,
        float a, float b, float c,
        int16_t brightness, float contrast,
        uint8_t *result, int noT) {

    size_t total_pixels = width * height;
    int optimal_thread_count = 1; // Find the optimal thread count (1 to 10)
    
    /*
    for (int i = numThreads; i > 0; --i) {
        if (total_pixels % i == 0) {
            optimal_thread_count = i;
            break;
        }
    }
    */
    optimal_thread_count=noT;




    pthread_t threads[optimal_thread_count];
    ThreadData thread_data[optimal_thread_count];

    size_t chunk_size = total_pixels / optimal_thread_count;

    // 1st subtask: Calculate Q Primes and save in result
    for (int i = 0; i < optimal_thread_count; ++i) {
        thread_data[i] = (ThreadData){
                .img = img,
                .result = result,
                .start = i * chunk_size,
                .end = (i + 1) * chunk_size,
                .a = a,
                .b = b,
                .c = c,
                .brightness = brightness,
                .contrast = contrast
        };
        pthread_create(&threads[i], NULL, calculate_q_primes, &thread_data[i]);
    }

    float totalSum = 0.0f;
    // Join threads to ensure completion of Q Primes calculation and accumulate sums for mean and variance
    for (int i = 0; i < optimal_thread_count; ++i) {
        pthread_join(threads[i], NULL);
    }
    for (size_t i = 0; i < total_pixels; i ++){
        totalSum += result[i];
    }
    // Calculate mean_brightened in the main thread
    float mean_brightened = totalSum / total_pixels;
    float variance_afterBrightness = 0.0f;
    // Calculate variance in the main thread
    for (size_t i = 0; i < total_pixels; ++i) {
        variance_afterBrightness += (result[i] - mean_brightened) * (result[i] - mean_brightened);
    }
    variance_afterBrightness /= total_pixels;
    // Calculate contrast factor
    float contrast_factor = (variance_afterBrightness == 0) ? 0 : contrast / sqrt(variance_afterBrightness);

    // 2nd subtask: Calculate Q Double Prime
    for (int i = 0; i < optimal_thread_count; ++i) {
        thread_data[i].mean_brightened = mean_brightened;
        thread_data[i].contrast_factor = contrast_factor;
        pthread_create(&threads[i], NULL, calculate_q_double_prime, &thread_data[i]);
    }

    // Join threads to ensure completion of Q Double Prime calculation
    for (int i = 0; i < optimal_thread_count; ++i) {
        pthread_join(threads[i], NULL);
    }
}
