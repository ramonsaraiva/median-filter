#ifndef IMAGE_H
#define IMAGE_H	1

#define THREADS 10

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct image_s
{
	int* pixels;
	int width;
	int height;
} image_t;

void image_new(image_t* i);
void image_size(image_t* i, int w, int h);
void image_set_pixel(image_t* img, int rgb, int x, int y);
int image_get_pixel(image_t* img, int x, int y);
void image_median_filter(image_t* img);
void* median_filter(void *number_void_ptr);
void image_copy(image_t *orig, image_t *dest);
void image_free(image_t *img);

#endif
