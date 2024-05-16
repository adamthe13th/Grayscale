#include "main.h"


void print_help() {
    printf("Usage: program [OPTIONS] <input_file>\n");
    printf("Options:\n");
    printf("  -V <number>           Specify the implementation (default: 0 || SIMD: 1 || Desaturation: 2 || multi-Threaded: 3 || LookUpTable: 4)\n");
    printf("  -n <number>           When running in V 3 Multi_Threaded, specify the number of Threads. Default is all available Threads - not Optimal!\n");
    printf("  -B <number>           Measure and output the runtime of the specified implementation\n");
    printf("                        with the given number of repetitions\n");
    printf("  -o <output_file>      Specify the output file\n");

    //here again the BT.709 standard is being used
    //https://www.color.org/chardata/rgb/BT709.xalter
    printf("  --coeffs <a>,<b>,<c>  Coefficients for grayscale conversion (default: a=0.2126, b=0.7152, c=0.0722)\n");
    printf("  --brightness <value>  Set brightness level in the range [-255, 255]\n");
    printf("  --contrast <value>    Set contrast level in the range [-255, 255]\n");
    printf("  -h, --help            Display this help and exit\n");
    printf("  -t <number>           Run tests to determine Optimal Number of Threads for current Machine/Image size Combination\n");
}

int main (int argc, char** argv) {
    int testingTh=0;    //number of selected Threads for running V 3
    int tRep=10; //number of iterations for the Thread Test
    int availableT= omp_get_num_procs(); //number of available HW Threads on current CPU
    int opt;
    int implementation = 0;
    int measure_runtime = 0; //bool for Benchmarking
    int noT=availableT;//number of Threads for V3 multithreaded implementation
    int repetitions = 1; //number of repititions/ iterations for Benchmarking
    char *input_file = "./inputs/4kinput.ppm";
    char *output_file = "output.pgm"; // default name if none is given
    //for coefs using the BT.709 standard
    float coeffs[3] = {0.2126, 0.7152, 0.0722};
    int brightness = 0;
    int contrast = 45;
    //vars for benchmarking
    clock_t start, end;
    double timeSum=0;
    double timeavg;
    struct option long_options[] = {
        {"coeffs", required_argument, NULL, 1},
        {"brightness", required_argument, NULL, 2},
        {"contrast", required_argument, NULL, 3},
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };
    //runs a multitude of test with benchmarking enabled
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--test") == 0 || strcmp(argv[i], "-T") == 0) {
            runAllTests();
            return 0;
        }
    }
    
    

        while ((opt = getopt_long(argc, argv, "V:o:hB:n:t:", long_options, NULL)) != -1) {
            //printf("opt is %c\n", opt);
            switch (opt) {
            case 'V':
                implementation = atoi(optarg);
                break;
            case 'B':
                measure_runtime = 1;
                repetitions = atoi(optarg);
                break;
            case 'o':
                output_file = optarg;
                break;
            case 1:
                sscanf(optarg, "%f,%f,%f", &coeffs[0], &coeffs[1], &coeffs[2]);
                break;
            case 2:
                brightness = atoi(optarg);
                break;
            case 3:
                contrast = atoi(optarg);
                break;
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
            case 'n':
                noT = atoi(optarg);
                break;
            case 't':
                testingTh=1;
                tRep=atoi(optarg);
                tRep = tRep <=0 ? 10 : tRep;
                implementation = 42069; //magic number for implementation switch to select Thread testing
                break;
            case '?':
                fprintf(stderr, "Error: Unknown option\n");
                print_help();
                exit(EXIT_FAILURE);
        }
    }
    // this is to check if the input file was given, otherwise error+exit

    if (optind + 1 < argc) {
        fprintf(stderr, "Error: More than one input file provided\n");
        print_help();
        exit(EXIT_FAILURE);
    } 
    else if (optind < argc){
        input_file = argv[optind];
    }
    else {
        fprintf(stderr, "Error: Missing input file\n");
        print_help();
        exit(EXIT_FAILURE);
    }
    //takes care fo reading the file
    Image inputImage = read_ppm(input_file);
    
    //controlling values for parameters
    if(brightness>255 || brightness<-255) {
        printf("Brightness value not in [-255, 255] range. Please choose a value within the specified range.\n");
        exit(EXIT_FAILURE);
    }
    if(contrast>255 || contrast<-255) {
        printf("Contrast value not in [-255, 255] range. Please choose a value within the specified range\n");
        exit(EXIT_FAILURE);
    }
    if(repetitions<=0) {
        printf("Benchmarking enabled but value for iterations not valid. Please choose a value >=1.\nDisplaying benchmarking for 1 iteration\n");
        repetitions=1;
    }
    if(noT<=0) {
        printf("Number of Threads must be >0\n");
        exit(EXIT_FAILURE);
    }

    //allocating space for the result
    uint8_t *result = (uint8_t *) malloc(inputImage.width * inputImage.height);
    if (result == NULL) {
        fprintf(stderr, "Failed to allocate memory for the result image\n");
        free_image(&inputImage);
        return EXIT_FAILURE;
    }
    
    
    
    
    
    if(measure_runtime && (testingTh==0)) {
        printf("\nBenchmarking enabled and running %d iterations ...\n", repetitions);
    }

    //calling correct implementation /or test
    switch (implementation)
    {
    case 1://SIMD
    for(size_t i = 0; i<repetitions; i++) {
        start=clock();
        brightness_contrast_V1(inputImage.pixels, inputImage.width, inputImage.height, coeffs[0], coeffs[1], coeffs[2], brightness, contrast, result );
        end=clock();
        timeSum += (double) (end-start)/CLOCKS_PER_SEC;
    }
        break;
    case 2://desaturation
    for(size_t i = 0; i<repetitions; i++) {
        start=clock();
        brightness_contrast_V2(inputImage.pixels, inputImage.width, inputImage.height, coeffs[0], coeffs[1], coeffs[2], brightness, contrast, result );
        end=clock();
        timeSum += (double) (end-start)/CLOCKS_PER_SEC;
    }
        break;
    case 0://default
    for(size_t i = 0; i<repetitions; i++) {
        start=clock();
        brightness_contrast(inputImage.pixels, inputImage.width, inputImage.height, coeffs[0], coeffs[1], coeffs[2], brightness, contrast, result );
        end=clock();
        timeSum += (double) (end-start)/CLOCKS_PER_SEC;
    }
        break;
    case 3://threaded
    printf("Available Threads: -------------%d\nRunning on: --------------------%d\n", omp_get_num_procs(), noT );
    for(size_t i = 0; i<repetitions; i++) {
        start=clock();
        brightness_contrast_V3(inputImage.pixels, inputImage.width, inputImage.height, coeffs[0], coeffs[1], coeffs[2], brightness, contrast, result , noT);
        end=clock();
        timeSum += (double) (end-start)/CLOCKS_PER_SEC;
    }
        break;
    case 4://lookup table
    for(size_t i = 0; i<repetitions; i++) {
        start=clock();
        brightness_contrast_V4(inputImage.pixels, inputImage.width, inputImage.height, coeffs[0], coeffs[1], coeffs[2], brightness, contrast, result );
        end=clock();
        timeSum += (double) (end-start)/CLOCKS_PER_SEC;
    }
        break;
    case 42069: //Thread test
        testingTh=1;
        double minTime = 100.0;
        int optThread=16;
        printf("Running threads test\nAvailable Threads: ------------------------------------%d\n", availableT);
    for (int j= 2; j<=availableT; j++) {
        timeSum=0;
        for(size_t i = 0; i<tRep; i++) {
            start=clock();
            brightness_contrast_V3(inputImage.pixels, inputImage.width, inputImage.height, coeffs[0], coeffs[1], coeffs[2], brightness, contrast, result , j);
            end=clock();
            timeSum += (double) (end-start)/CLOCKS_PER_SEC;
        }
        
        timeavg=timeSum/tRep;
        if(timeavg<minTime) {
            minTime=timeavg;
            optThread=j;
        }
        //printf("Ran on %d Threads for 10 iterations\nTotal Time: %f\nAverage Time: %f\n", j, timeSum, timeavg);
    }
    printf("Tests concluded,\nOptimal Thread count for this image size is: ----------%d\nFor an average of: ------------------------------------%f\n", optThread, minTime);
    printf("\n**When running V3, use Option -n <number> to specify the number of Threads used.\n");
        break;
    default: //none of the available options
        printf("The given Implementation cannot be found. Please choose (default: 0 || SIMD: 1 || Desaturation: 2 || multi-Threaded: 3 || LookUpTable: 4)");
        print_help();
        return EXIT_FAILURE;
    }

    //save result

    timeavg = timeSum/repetitions;
    Image output_Image;
    output_Image.height = inputImage.height;
    output_Image.width = inputImage.width;
    output_Image.pixels = result;
    write_pgm(output_file, &output_Image);
    if(measure_runtime && (testingTh==0)) {
        printf("Number of iterations: ----------%d\nTotal time: --------------------%fs\nAverage time: ------------------%fs\n", repetitions, timeSum, timeavg);
    }
    return EXIT_SUCCESS;
}

