CC = gcc
CFLAGS = -Wall -O3 -fopenmp
LIBS = -lm 

SRCS = main.c desaturation.c scalar_grayscale.c simd_grayscale.c image_io.c test_grayscale.c threaded_grayscale.c lookuptable_grayscale.c
OBJS = $(SRCS:.c=.o)

TARGET = grayscale

all: $(TARGET) clean

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJS)
cleanall:
	rm -f $(OBJS) $(TARGET)



