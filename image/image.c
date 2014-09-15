#include <stdlib.h>
#include <stdio.h>

#include "image/image.h"

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
	image_t* filter_img;

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

	filter_img = malloc(sizeof(image_t));
	image_new(filter_img);
	image_size(filter_img, img->width, img->height);

	// for each pixel
	for (i = 0; i < img->height; i++)
	{
		for (j = 0; j < img->width; j++)
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

					ay = i + y;
					ax = j + x;

					// check if pixel is in image
					if (ay >= 0 && ay < img->height && ax >= 0 && ax < img->width)
					{
						rgb = image_get_pixel(img, ax, ay);
						r_sum += (rgb) & 0xFF;
						g_sum += (rgb >> 8) & 0xFF;
						b_sum += (rgb >> 16) & 0xFF;
						rgb_count++;
					}
				}
			}

			rgb = (r_sum/rgb_count << 16) | (g_sum/rgb_count << 8) | b_sum/rgb_count | (255 << 24);
			image_set_pixel(filter_img, rgb, j, i);
		}
	}

	memcpy(img, filter_img, sizeof(image_t));
	free(filter_img);
}
