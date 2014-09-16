#include <stdio.h>
#include <stdlib.h>

#include "image/image.h"

int load_ppm(char* p, image_t* img)
{
	FILE* in;
	char buffer[128];

	int i;
	int j;

	int width;
	int height;
	int max;

	int r;
	int g;
	int b;
	int rgb;

	in = fopen(p, "r");

	if (!in)
		return -1;

	fgets(buffer, 128, in);

	if (buffer[0] == 'P' && buffer[1] == '3')
	{
		do
		{
			fgets(buffer, 128, in);
		}
		while (buffer[0] == '#');

		sscanf(buffer, "%d %d", &width, &height);

		printf("WIDTH: %d \nHEIGHT: %d\n", width, height);

		fgets(buffer, 128, in);

		max = atoi(buffer);

		printf("MAX: %d\n", max);

		image_size(img, width, height);

		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				fgets(buffer, 128, in);
				r = atoi(buffer);

				fgets(buffer, 128, in);
				g = atoi(buffer);

				fgets(buffer, 128, in);
				b = atoi(buffer);

				rgb = (r << 16) | (g << 8) | b | (255 << 24);
				image_set_pixel(img, rgb, j, height - i - 1);

				//printf("PIXEL: R %d G %d B %d\n", r, g, b);
			}
		}
	}

	fclose(in);
}

int save_ppm(char* p, image_t* img)
{
	FILE* out;

	int i;
	int j;

	int r;
	int g;
	int b;
	int rgb;

	out = fopen(p, "w");

	fprintf(out, "P3\n");
	fprintf(out, "# CREATOR: MEDIAN FILTER - RAMON & LUIZ [UNISINOS]\n");
	fprintf(out, "%d %d\n", img->width, img->height);
	fprintf(out, "255\n");

	for (i = 0; i < img->height; i++)
	{
		for (j = 0; j < img->width; j++)
		{
			rgb = image_get_pixel(img, j, img->height - i - 1);
			r = (rgb) & 0xFF;
			g = (rgb >> 8) & 0xFF;
			b = (rgb >> 16) & 0xFF;

			fprintf(out, "%d\n", r);
			fprintf(out, "%d\n", g);
			fprintf(out, "%d\n", b);
		}
	}
	fclose(out);
}
