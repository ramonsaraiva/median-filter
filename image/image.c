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
	int i, side1, side2;
	dest_img = malloc(sizeof(image_t));
	orig_img = img;

	get_sides(&side1, &side2);
	if (THREADS > side1) {
		char error[1024];
		sprintf(error,"Number of threads(%d) is grather than %d!\n",THREADS, side1);
		perror(error);
		exit(1);
	}
	printf("MODE: %d\n",mode);

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

void static get_sides(int* side1, int* side2)
{
	if (orig_img->width >= orig_img->height)
	{
		*side1 = orig_img->width;
		*side2 = orig_img->height;
		mode = NORMAL;
	}
	else
	{
		*side1 = orig_img->height;
		*side2 = orig_img->width;
		mode = REVERSE;
	}
}

void* median_filter(void *number_void_ptr)
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

	int width;
	int height;
	int initial;
	int end;
	int number = (int)number_void_ptr;

	get_sides(&width, &height);

	initial = width / THREADS * (number);
	end = width / THREADS * (number + 1);
	if (number+1 == THREADS)
	{
		end = width;
	}

	printf("Thread #%d: init -> %d, end -> %d\n", number+1, initial, end);

	// for each pixel
	for (i = 0; i < height; i++)
	{
		for (j = initial; j < end; j++)
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
					if (ay >= 0 && ay < height && ax >= 0 && ax < width)
					{
						if (mode == NORMAL)
						{
							rgb = image_get_pixel(orig_img, ax, ay);
						}
						else
						{
							rgb = image_get_pixel(orig_img, ay, ax);
						}
						r_sum += (rgb) & 0xFF;
						g_sum += (rgb >> 8) & 0xFF;
						b_sum += (rgb >> 16) & 0xFF;
						rgb_count++;
					}
				}
			}

			rgb = (r_sum/rgb_count << 16) | (g_sum/rgb_count << 8) | b_sum/rgb_count | (255 << 24);
			if (mode == NORMAL)
			{
				image_set_pixel(dest_img, rgb, j, i);
			}
			else
			{
				image_set_pixel(dest_img, rgb, i, j);
			}
		}
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

