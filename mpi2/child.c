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

	MPI_Comm_get_parent(&parent);

	MPI_Recv(&pixels_per_proc, 1, MPI_INT, 0, 0, parent, &status);
	MPI_Recv(&limits, 2, MPI_INT, 0, 0, parent, &status);
	MPI_Recv(&img_size, 1, MPI_INT, 0, 0, parent, &status);
	MPI_Recv(&img_width, 1, MPI_INT, 0, 0, parent, &status);
	MPI_Recv(&img_height, 1, MPI_INT, 0, 0, parent, &status);

	img.pixels = malloc(img_size * sizeof(int));
	MPI_Recv(img.pixels, img_size, MPI_INT, 0, 0, parent, &status);

	printf("[RANK %d] LIMITS! %d ~ %d\n", rank, limits[0], limits[1]);

	int limits_size = limits[1] - limits[0] + 1;
	int pix[limits_size];

	if (limits_size < pixels_per_proc)
	{
//		i can do all the job!
		int i;

		img.width = img_width;
		img.height = img_height;

		image_median_filter(&img, limits, pix);
	}
	else
	{
		int c_limits[4];

		c_limits[0] = 0;
		c_limits[1] = (limits_size/2) - 1;
		c_limits[2] = limits_size/2;
		c_limits[3] = limits_size-1;

		int errcode;
		MPI_Comm a_inter_comm, b_inter_comm;

		MPI_Comm_spawn("build/bin/child", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &a_inter_comm, &errcode);
		MPI_Comm_spawn("build/bin/child", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &b_inter_comm, &errcode);

		/* SEND LIMITS TO CHILDREN */
		MPI_Send(&pixels_per_proc, 1, MPI_INT, 0, 0, a_inter_comm);
		MPI_Send(&c_limits, 2, MPI_INT, 0, 0, a_inter_comm);
		MPI_Send(&img_size, 1, MPI_INT, 0, 0, a_inter_comm);
		MPI_Send(&img_width, 1, MPI_INT, 0, 0, a_inter_comm);
		MPI_Send(&img_height, 1, MPI_INT, 0, 0, a_inter_comm);
		MPI_Send(img.pixels, img_size, MPI_INT, 0, 0, a_inter_comm);

		MPI_Send(&pixels_per_proc, 1, MPI_INT, 0, 0, b_inter_comm);
		MPI_Send(&c_limits[2], 2, MPI_INT, 0, 0, b_inter_comm);
		MPI_Send(&img_size, 1, MPI_INT, 0, 0, b_inter_comm);
		MPI_Send(&img_width, 1, MPI_INT, 0, 0, b_inter_comm);
		MPI_Send(&img_height, 1, MPI_INT, 0, 0, b_inter_comm);
		MPI_Send(img.pixels, img_size, MPI_INT, 0, 0, b_inter_comm);

		MPI_Status status;
		MPI_Recv(&pix[c_limits[0]], limits_size/2, MPI_INT, 0, 0, a_inter_comm, &status);
		MPI_Recv(&pix[c_limits[2]], limits_size/2, MPI_INT, 0, 0, b_inter_comm, &status);
	}

	MPI_Send(pix, limits_size, MPI_INT, 0, 0, parent);

	MPI_Finalize();
}
