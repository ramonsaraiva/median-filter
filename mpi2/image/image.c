#include "image/image.h"
int mode;
int thread_n;

void image_new(image_t* i)
{
	memset(i, 0, sizeof(image_t));
}

void image_size(image_t* i, int w, int h)
{
	i->width = w;
	i->height = h;

	i->pixels = malloc(MPI_INT * w * h);
}

void image_set_pixel(image_t* img, int rgb, int x, int y)
{
	img->pixels[x + y * img->width] = rgb;
}

int image_get_pixel(image_t* img, int x, int y)
{
	return img->pixels[x + y * img->width];
}

void image_median_filter(image_t* orig, int limits[2], int* pixels)
{
	int rgb;
	int r_sum;
	int g_sum;
	int b_sum;
	int rgb_count;

	int i;
	int j;
	int x;
	int y;
	int ax;
	int ay;
	int initial;
	int end;
	int rank;
	int size;

	initial = limits[0];
	end = limits[1];

	printf(".. Median filter between %d and %d!\n", initial, end);

	int k = 0;
	for (j = initial; j <= end; j++)
	{
		r_sum = 0;
		g_sum = 0;
		b_sum = 0;
		rgb_count = 0;

		// for each pixel in 3x3 mask
		for (y = -1; y <= 1; y++)
		{
			for (x = -1; x <= 1; x++)
			{
				// current pixel, so just continue
				if (y == 1 && x == 1)
					continue;

				ay = j/orig->width + y;
				ax = j%orig->width + x;

				// check if pixel is in image
				if (ay >= 0 && ay < orig->height && ax >= 0 && ax < orig->width)
				{
					rgb = image_get_pixel(orig, ax, ay);
					r_sum += (rgb) & 0xFF;
					g_sum += (rgb >> 8) & 0xFF;
					b_sum += (rgb >> 16) & 0xFF;
					rgb_count++;
				}
			}
		}

		rgb = (r_sum/rgb_count << 16) | (g_sum/rgb_count << 8) | b_sum/rgb_count | (255 << 24);
		pixels[k++] = rgb;
	}
}

void image_copy(image_t *orig, image_t *dest) {
	image_free(dest);
	image_new(dest);
	image_size(dest, orig->width, orig->height);
	memcpy(dest->pixels, orig->pixels, dest->width*dest->height*MPI_INT);
}

void image_free(image_t *img) {
	free(img->pixels);
	img = NULL;
}

