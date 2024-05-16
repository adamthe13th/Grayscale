#include "desaturation.h"


//keep in mind this function completely ignores Coeffs and uses the desaturation method!!

void brightness_contrast_V2(
       const uint8_t *img, size_t width, size_t height,
       float a, float b, float c,
       int16_t brightness, float contrast,
       uint8_t *result) {
            size_t j=0;
            size_t pixelCount = width*height;
            size_t subPixelCount = pixelCount*3;
            size_t sum=0;
            for(int i = 0; i< subPixelCount; i+=3) {
                uint8_t maxRGB = max(img[i], max(img[i+1], img[i+2]));
                uint8_t minRGB = min(img[i], min(img[i+1], img[i+2]));
                uint8_t D = CLAMP((int)((maxRGB+minRGB)/2+brightness));
                //adding sum to calculate average value (after brightness)
                sum+=D;
                result[j] = D;
                j++;
            }
            
            //divide by number of pixels for average (after brightness)
            size_t average = (size_t) sum/pixelCount;
            
            // zeroing out sum to reuse
            sum =0;
            // iterate over all pixels to sum square of difference between pixel and average for total variance
            for(int i =0; i<pixelCount; i++) {
                sum+= (result[i]-average)*(result[i]-average);
            }
            // calculate variance
            float variance = sqrt(sum/pixelCount);

            if (contrast==0 && variance ==0){
            //calculating final Q" for contrast=variance=0 (simplified)
                for(int i =0; i<pixelCount; i++) {
                    result[i] = average;
                }
            } else { //if not
                for(int i =0; i<pixelCount; i++) {
                    result[i] = CLAMP((int)(contrast*result[i]/variance + (1-contrast/variance)* average));
                }
                
            }
            
       }