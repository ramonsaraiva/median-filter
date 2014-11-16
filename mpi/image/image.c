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

void image_median_filter(image_t* img)
{
	int i, side1, side2;

	int rank, size;
	MPI_Status status;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */


	if (rank == 0) {
		int limits[size*2];
		for (i=0; i < size-1; i++) {
			int pos = (i)*2;
			limits[pos] = img->width / (size-1) * (i);
			if (i+1 == size-1) {
				limits[pos+1] = img->width;
			} else {
				limits[pos+1] = img->width / (size-1) * (i + 1);
			}
			MPI_Send(&limits[i*2], 2, MPI_INT, i+1, 0, MPI_COMM_WORLD);
		}
		//printf("END OF SEND - INIT OF RECV\n");
		for (i=0; i< size-1; i++) {
			int pos = (i)*2;
			int pSize = (limits[pos+1] - limits[pos]) * img->height;
			int pixels[pSize];
//			printf("%d: %d, %d => %d\n", i+1, limits[pos], limits[pos+1], pSize);
			MPI_Recv(&pixels, pSize, MPI_INT, i+1, 0, MPI_COMM_WORLD, &status);
			int j;
			int k;
			int l =0;
			//printf("%d, %d\n", limits[pos], limits[pos+1]);
			for (k = 0; k < img->height; k++) {
				for (j = limits[pos]; j < limits[pos+1]; j++) {
					image_set_pixel(img, pixels[l++], j, k);
				}
			}
		}
	} else {
		int limits[2];
		MPI_Recv(&limits, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		median_filter(img, limits);
	}

}

void median_filter(image_t* orig, int limits[2])
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


	initial = limits[0];
	end = limits[1];

	int rank;
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
	int size = (end-initial) * orig->height;
	int pixels[size];

	//printf("Thread #%d: init -> %d, end -> %d\n", number+1, initial, end);

	// for each pixel
	int k = 0;
	for (i = 0; i < orig->height; i++)
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
			//image_set_pixel(dest, rgb, j, i);
		}
	}

//  printf("%d: %d -> %d = %d\n", rank, initial, end, size);
	MPI_Send(&pixels, size, MPI_INT, 0, 0, MPI_COMM_WORLD);
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

