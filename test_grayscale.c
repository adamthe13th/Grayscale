#include "test_grayscale.h"

//let's set the value to 0.1 for error tolerance
#define TOLERANCE 0.1
#define SOME_THRESHOLD 20


// Function prototypes
//void runTestCases();
void run_command(const char *command);

void run_command(const char *command) {
    printf("Executing command: %s\n", command);
    int result = system(command);

    if (result != 0) {
        printf("An error occurred while executing the command.\n");
    } else {
        printf("Command executed successfully.\n");
    }

    printf("\n");
}


void testOutputFile(const char* outputFile, uint8_t expectedBrightness) {
    FILE *file = fopen(outputFile, "rb");
    if (!file) {
        perror("Failed to open output file for verification");
        return;
    }

    int width = 0;
    int height = 0;
    // Skip the header of the PGM file
    if (fscanf(file, "P5\n%d %d\n255\n", &width, &height) != 2) {
        perror("Failed to read image dimensions");
        fclose(file);
        return;
    }

    int match = 1;
    uint8_t pixel;
    for (int i = 0; i < width * height; i++) {
        if (fread(&pixel, 1, 1, file) != 1) {
            perror("Failed to read pixel data");
            match = 0;
            break;
        }
        if (pixel != expectedBrightness) {
            printf("Mismatch found at pixel %d: Actual Value = %d, Expected Value = %d\n", i, pixel, expectedBrightness);
            match = 0;
            break;
        }
    }

    fclose(file);

    if (match) {
        printf("Test Passed: Output file matches the expected brightness level.\n");
    } else {
        printf("Test Failed: Output file does not match the expected brightness level.\n");
    }
}

void testBrightnessBounds() {
    //Check for the brighntess: 255, contrast: no change
    //This means that all teh pixels in the output image would be
    //255 and the image is white
    run_command("./grayscale -V 0 --brightness 255 --contrast 0 -o ../Implementierung/outputs/output_test_white0.pgm ../Implementierung/inputs/input.ppm");
    testOutputFile("../Implementierung/outputs/output_test_white0.pgm", 255);
    
    //similar file above but now the value of each pixel should is black
    run_command("./grayscale -V 0 --brightness -255 --contrast 0 -o ../Implementierung/outputs/output_test_black0.pgm ../Implementierung/inputs/input.ppm");
    //simliar lile above but now the value of each pixel should is black
    run_command("./grayscale -V 0 --brightness -255 --contrast 0 -o ../Implementierung/outputs/output_test_black0.pgm ../Implementierung/inputs/input.ppm");
    testOutputFile("../Implementierung/outputs/output_test_black0.pgm", 0);

    //we do the same thing for the other two implementations
    run_command("./grayscale -V 1 --brightness 255 --contrast 0 -o ../Implementierung/outputs/output_test_white1.pgm ../Implementierung/inputs/input.ppm");
    testOutputFile("../Implementierung/outputs/output_test_white1.pgm", 255);
    
    //simliar lile above but now the value of each pixel should is black
    run_command("./grayscale -V 1 --brightness -255 --contrast 0 -o ../Implementierung/outputs/output_test_black1.pgm ../Implementierung/inputs/input.ppm");
    testOutputFile("../Implementierung/outputs/output_test_black1.pgm", 0);

    run_command("./grayscale -V 2 --brightness 255 --contrast 0 -o ../Implementierung/outputs/output_test_white2.pgm ../Implementierung/inputs/input.ppm");
    testOutputFile("../Implementierung/outputs/output_test_white2.pgm", 255);
    
    //simliar lile above but now the value of each pixel should is black
    run_command("./grayscale -V 2 --brightness -255 --contrast 0 -o ../Implementierung/outputs/output_test_black2.pgm ../Implementierung/inputs/input.ppm");
    testOutputFile("../Implementierung/outputs/output_test_black2.pgm", 0);

    run_command("./grayscale -V 3 --brightness 255 --contrast 0 -o ../Implementierung/outputs/output_test_white3.pgm ../Implementierung/inputs/input.ppm");
    testOutputFile("../Implementierung/outputs/output_test_white3.pgm", 255);

    //similar file above but now the value of each pixel should is black
    run_command("./grayscale -V 3 --brightness -255 --contrast 0 -o ../Implementierung/outputs/output_test_black3.pgm ../Implementierung/inputs/input.ppm");
    testOutputFile("../Implementierung/outputs/output_test_black3.pgm", 0);


    run_command("./grayscale -V 4 --brightness 255 --contrast 0 -o ../Implementierung/outputs/output_test_white4.pgm ../Implementierung/inputs/input.ppm");
    testOutputFile("../Implementierung/outputs/output_test_white4.pgm", 255);

    //similar file above but now the value of each pixel should is black
    run_command("./grayscale -V 4 --brightness -255 --contrast 0 -o ../Implementierung/outputs/output_test_black4.pgm ../Implementierung/inputs/input.ppm");
    testOutputFile("../Implementierung/outputs/output_test_black4.pgm", 0);

}


uint8_t* readPGMImage(const char* filename, int* width, int* height) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    // Check the number of successful matches
    if (fscanf(file, "P5\n%d %d\n255\n", width, height) != 2) {
        perror("Failed to read image dimensions");
        fclose(file);
        return NULL;
    }

    size_t pixelCount = (*width) * (*height);

    uint8_t* pixels = (uint8_t*)malloc(pixelCount);
    if (pixels == NULL) {
        fprintf(stderr, "Failed to allocate memory for the result image\n");
        return NULL;
    }

    if (fread(pixels, 1, pixelCount, file) != pixelCount) {
        perror("Failed to read pixel data");
        fclose(file);
        free(pixels);
        return NULL;
    }
    fclose(file);
    return pixels;
}



//we call the image to be flat, when all the pixels have same value
void checkFlatImage(const char* outputFile) {
    int width, height;
    uint8_t* pixels = readPGMImage(outputFile, &width, &height);

    if (pixels == NULL) {
        printf("Failed to read the image data from %s.\n", outputFile);
        return;
    }

    uint8_t firstPixelValue = pixels[0];
    int isFlat = 1; // Assume the image is flat until proven otherwise
    size_t pixelCount = width * height;

    for (size_t i = 1; i < pixelCount; ++i) {
        if (pixels[i] != firstPixelValue) {
            isFlat = 0; // Image is not flat
            break;
        }
    }

    free(pixels);

    if (isFlat) {
        printf("Test Passed: The image is flat. All pixels have the value %d.\n", firstPixelValue);
    } else {
        printf("Test Failed: The image is not flat. Pixel values are not consistent.\n");
    }
}

void testContrast() {
    run_command("./grayscale -V 4 --brightness 0 --contrast 0 -o ../Implementierung/outputs/output_null_contrast4.pgm ../Implementierung/inputs/input.ppm");
    //the image here should be flat since the contrast is set to 0
    checkFlatImage("../Implementierung/outputs/output_null_contrast4.pgm");

    run_command("./grayscale -V 3 --brightness 0 --contrast 0 -o ../Implementierung/outputs/output_null_contrast3.pgm ../Implementierung/inputs/input.ppm");
    //the image here should be flat since the contrast is set to 0
    checkFlatImage("../Implementierung/outputs/output_null_contrast3.pgm");

    run_command("./grayscale -V 2 --brightness 0 --contrast 0 -o ../Implementierung/outputs/output_null_contrast2.pgm ../Implementierung/inputs/input.ppm");
    //the image here should be flat since the contrast is set to 0
    checkFlatImage("../Implementierung/outputs/output_null_contrast2.pgm");


    run_command("./grayscale -V 1 --brightness 0 --contrast 0 -o ../Implementierung/outputs/output_null_contrast1.pgm ../Implementierung/inputs/input.ppm");
    //the image here should be flat since the contrast is set to 0
    checkFlatImage("../Implementierung/outputs/output_null_contrast1.pgm");

    run_command("./grayscale -V 0 --brightness 0 --contrast 0 -o ../Implementierung/outputs/output_null_contrast0.pgm ../Implementierung/inputs/input.ppm");
    //the image here should be flat since the contrast is set to 0
    checkFlatImage("../Implementierung/outputs/output_null_contrast0.pgm");
}


void testMaxPositiveContrast(int algorithm) {
    char command[256];
    snprintf(command, sizeof(command), "./grayscale -V %d --brightness 0 --contrast 255 -o ../Implementierung/outputs/output_max_contrast.pgm ../Implementierung/inputs/input.ppm", algorithm);
    run_command(command);

    int width, height;
    uint8_t* pixels = readPGMImage("../Implementierung/outputs/output_max_contrast.pgm", &width, &height);
    if (pixels == NULL) return;

    size_t pixelCount = width * height;
    size_t lowCount = 0, highCount = 0;
    for (size_t i = 0; i < pixelCount; ++i) {
        if (pixels[i] < 50) lowCount++;      // Near black
        else if (pixels[i] > 205) highCount++; // Near white
    }

    free(pixels);

    if (lowCount + highCount > pixelCount/2) {
        printf("Test Passed: High contrast achieved.\n");
    } else {
        printf("Test Failed: High contrast not achieved.\n");
    }
}

void testMaxNegativeContrast(int algorithm) {
    char command[256];
    snprintf(command, sizeof(command), "./grayscale -V %d --brightness 0 --contrast -255 -o ../Implementierung/outputs/output_min_contrast.pgm ../Implementierung/inputs/input.ppm", algorithm);
    run_command(command);

    int width, height;
    uint8_t* pixels = readPGMImage("../Implementierung/outputs/output_min_contrast.pgm", &width, &height);
    if (pixels == NULL) return;

    size_t pixelCount = width * height;
    size_t lowCount = 0, highCount = 0;
    for (size_t i = 0; i < pixelCount; ++i) {
        if (pixels[i] < 50) lowCount++;      // Near black
        else if (pixels[i] > 205) highCount++; // Near white
    }

    free(pixels);

    if (lowCount + highCount > pixelCount/2) {
        printf("Test Passed: Low contrast achieved.\n");
    } else {
        printf("Test Failed: Low contrast not achieved.\n");
    }
}







void runAllTests() {
    printf("Running Brightness Bounds Tests...\n");
//    Test 1:
//    Checked for Brightness extreme values
    testBrightnessBounds();

    //Test 2:
    //Checked for 0 contrast which would make the image flat
    testContrast();

    //Test 3:
    //Checked for high contrast here which would make most of the pixels close to each other
    testMaxPositiveContrast(0);
    testMaxPositiveContrast(1);
    testMaxPositiveContrast(2);
    testMaxPositiveContrast(3);
    testMaxPositiveContrast(4);

    
    //Test 4:
    //Checked for low contrast here which also takes the pixel close to each other
    testMaxNegativeContrast(0);
    testMaxNegativeContrast(1);
    testMaxNegativeContrast(2);
    testMaxNegativeContrast(3);
    testMaxNegativeContrast(4);


    printf("running all implementations in Benchmarking mode\n");
    printf("using 256x256.ppm\n");
    run_command("./grayscale ./inputs/256x256.ppm -B 100 ");
    run_command("./grayscale ./inputs/256x256.ppm -B 100 -V 1");
    run_command("./grayscale./inputs/256x256.ppm -B 100 -V 2");
    run_command("./grayscale ./inputs/256x256.ppm -B 100 -V 3");
    run_command("./grayscale ./inputs/256x256.ppm -B 100 -V 4");
    run_command("./grayscale ./inputs/256x256.ppm -t 100");
    printf("using 512x512.ppm\n");
    run_command("./grayscale ./inputs/512x512.ppm -B 100 ");
    run_command("./grayscale ./inputs/512x512.ppm -B 100 -V 1");
    run_command("./grayscale ./inputs/512x512.ppm -B 100 -V 2");
    run_command("./grayscale ./inputs/512x512.ppm -B 100 -V 3");
    run_command("./grayscale ./inputs/512x512.ppm -B 100 -V 4");
    run_command("./grayscale ./inputs/512x512.ppm -t 100");
    printf("using 1024x1024.ppm\n");
    run_command("./grayscale ./inputs/1024x1024.ppm -B 100 ");
    run_command("./grayscale ./inputs/1024x1024.ppm -B 100 -V 1");
    run_command("./grayscale ./inputs/1024x1024.ppm -B 100 -V 2");
    run_command("./grayscale ./inputs/1024x1024.ppm -B 100 -V 3");
    run_command("./grayscale ./inputs/1024x1024.ppm -B 100 -V 4");
    run_command("./grayscale ./inputs/1024x1024.ppm -t 100");
    printf("using 2048x2048.ppm\n");
    run_command("./grayscale ./inputs/2048x2048.ppm -B 100 ");
    run_command("./grayscale ./inputs/2048x2048.ppm -B 100 -V 1");
    run_command("./grayscale ./inputs/2048x2048.ppm -B 100 -V 2");
    run_command("./grayscale ./inputs/2048x2048.ppm -B 100 -V 3");
    run_command("./grayscale ./inputs/2048x2048.ppm -B 100 -V 4");
    run_command("./grayscale ./inputs/2048x2048.ppm -t 100");
    
    printf("using 4096x4096.ppm\n");
    run_command("./grayscale ./inputs/4096x4096.ppm -B 100 ");
    run_command("./grayscale ./inputs/4096x4096.ppm -B 100 -V 1");
    run_command("./grayscale ./inputs/4096x4096.ppm -B 100 -V 2");
    run_command("./grayscale ./inputs/4096x4096.ppm -B 100 -V 3");
    run_command("./grayscale ./inputs/4096x4096.ppm -B 100 -V 4");
    run_command("./grayscale ./inputs/4096x4096.ppm -t 100");
    run_command("rm output.pgm");
    printf("All tests completed.\n");
}

