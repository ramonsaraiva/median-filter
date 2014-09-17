#include "image/image.h"
image_t* dest_img;
image_t* orig_img;
int mode;

void image_new(image_t* i)
{
	memset(i, 0, sizeof(image_t));
}

void image_size(image_t* i, int w, int h)
{
	i->width = w;
	i->height = h;

	i->pixels = malloc(sizeof(int) * w * h);
}

void image_set_pixel(image_t* img, int rgb, int x, int y)
{
	img->pixels[x + y * img->width] = rgb;
}

int image_get_pixel(image_t* img, int x, int y)
{
	return img->pixels[x + y * img->width];
}

void image_median_filter(image_t* img)
{
	int i, size;
	dest_img = malloc(sizeof(image_t));
	orig_img = img;
	size = img->width * img->height;

	if (THREADS > size) {
		char error[1024];
		sprintf(error,"Number of threads(%d) is grather than %d!\n",THREADS, size);
		perror(error);
		exit(1);
	}

	image_new(dest_img);
	image_size(dest_img, img->width, img->height);

	pthread_t median_thread[THREADS];

	for (i=0; i < THREADS; i++)
	{
		pthread_create(&median_thread[i],NULL,median_filter, (void*)i);
	}
	for (i=0; i < THREADS; i++)
	{
		pthread_join(median_thread[i], NULL);
	}
	image_copy(dest_img, img);
	image_free(dest_img);
	free(dest_img);
}

void* median_filter(void *number_void_ptr)
{
	int rgb;
	int r_sum;
	int g_sum;
	int b_sum;
	int rgb_count;

	int i;
	int x;
	int y;
	int ax;
	int ay;

	int size;
	int initial;
	int end;
	int number = (int)number_void_ptr;

	size = orig_img->width * orig_img->height;

	initial = size / THREADS * (number);
	end = size / THREADS * (number + 1);
	if (number+1 == THREADS)
	{
		end = size;
	}

	printf("Thread #%d: init -> %d, end -> %d\n", number+1, initial, end);
	int medianX, medianY;
	medianX = medianY = 5;

	// for each pixel
	for (i = initial; i < end; i++)
	{
		int column, line;
		line = i / orig_img->width;
		column = i % orig_img->width;
		r_sum = 0;
		g_sum = 0;
		b_sum = 0;
		rgb_count = 0;

		// for each pixel in 3x3 mask
		for (x = 0 - (medianX/2); x <= medianX/2; x++)
		{
			for (y = 0 - (medianY/2); y <= medianY/2; y++)
			{
				// current pixel, so just continue
/*				if (y == 0 && x == 0)
					continue;*/
				ay = line + y;
				ax = column + x;
				rgb_count++;
				// check if pixel is in image
				if (ay >= 0 && ay < orig_img->height && ax >= 0 && ax < orig_img->width)
				{
					rgb = image_get_pixel(orig_img, ax, ay);
					r_sum += (rgb) & 0xFF;
					g_sum += (rgb >> 8) & 0xFF;
					b_sum += (rgb >> 16) & 0xFF;
				}
			}
		}
		rgb = (r_sum/rgb_count << 16) | (g_sum/rgb_count << 8) | b_sum/rgb_count | (255 << 24);
		image_set_pixel(dest_img, rgb, column, line);
	}
}

void image_copy(image_t *orig, image_t *dest) {
	image_free(dest);
	image_new(dest);
	image_size(dest, orig->width, orig->height);
	memcpy(dest->pixels, orig->pixels, dest->width*dest->height*sizeof(int));
}

void image_free(image_t *img) {
	free(img->pixels);
	img = NULL;
}

