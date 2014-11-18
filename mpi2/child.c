#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mpi.h>

#include "image/image.h"
#include "loader/loader.h"

int main(int argc, char** argv)
{
	int rank, size;
	int limits[2];
	int pixels_per_proc;
	int img_size;
	int img_width;
	int img_height;
	int* pixels;

	image_t img;
	image_new(&img);

	MPI_Comm parent;
	MPI_Status status;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	printf("Hello, child spawned: rank %d size %d\n", rank, size);

	MPI_Comm_get_parent(&parent);

	MPI_Recv(&pixels_per_proc, 1, MPI_INT, 0, 0, parent, &status);
	MPI_Recv(&limits, 2, MPI_INT, 0, 0, parent, &status);
	MPI_Recv(&img_size, 1, MPI_INT, 0, 0, parent, &status);
	MPI_Recv(&img_width, 1, MPI_INT, 0, 0, parent, &status);
	MPI_Recv(&img_height, 1, MPI_INT, 0, 0, parent, &status);

	pixels = malloc(img_size * sizeof(int));
	MPI_Recv(pixels, img_size, MPI_INT, 0, 0, parent, &status);

	printf("[RANK %d] Hey i got my limits! %d ~ %d\n", rank, limits[0], limits[1]);

	if (limits[1] - limits[0] + 1 < pixels_per_proc)
	{
//		i can do all the job!
		int i;

		printf("I can do it alone\n");
		img.width = img_width;
		img.height = img_height;
		img.pixels = pixels;

		image_median_filter(&img, limits, pixels);

		for (i = limits[0]; i <= limits[1]; i++)
			image_set_pixel(&img, pixels[i], i%img.width, i/img.width);
	}
	else
	{
		printf("else?\n");
	}

	printf("i will send %d ints\n", limits[1] - limits[0] + 1);
	MPI_Send(pixels[limits[0]], limits[1] - limits[0] + 1, MPI_INT, 0, 0, parent);

	MPI_Finalize();
}
