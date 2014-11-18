#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <unistd.h>

#include "image/image.h"
#include "loader/loader.h"

#include <mpi.h>
#include "exectime/exectime.h"

unsigned long tms (void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int main(int argc, char** argv)
{
	MPI_Init (&argc, &argv);      /* starts MPI */

	unsigned long tic;
	unsigned long toc;

	float time_one_thread;
	int rank, size;

	int pixels_per_proc;
	int img_size;
	int limits[2];

	image_t img;
	image_new(&img);

	load_ppm(argv[1], &img);

	MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

	pixels_per_proc = atoi(argv[3]);
	img_size = img.width * img.height;

	printf("Pixels per proc: %d\n", pixels_per_proc);
	printf("Image res: %dx%d\n", img.width, img.height);
	printf("Image size: %d\n", img_size);

	limits[0] = 0;
	limits[1] = img_size - 1;
	int pixels[limits[1] - limits[0] + 1];

	if (img_size < pixels_per_proc)
	{
		image_median_filter(&img, limits, pixels);
	}
	else
	{
		int c_limits[4];

		c_limits[0] = 0;
		c_limits[1] = (img_size/2) - 1;
		c_limits[2] = img_size/2;
		c_limits[3] = img_size-1;

		printf("Children values => A %d ~ %d .. B %d ~ %d\n", c_limits[0], c_limits[1], c_limits[2], c_limits[3]);

		int errcode;
		MPI_Comm a_inter_comm, b_inter_comm;

		MPI_Comm_spawn("build/bin/child", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &a_inter_comm, &errcode);
		MPI_Comm_spawn("build/bin/child", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &b_inter_comm, &errcode);

		/* SEND LIMITS TO CHILDREN */
		MPI_Send(&pixels_per_proc, 1, MPI_INT, 0, 0, a_inter_comm);
		MPI_Send(&c_limits, 2, MPI_INT, 0, 0, a_inter_comm);
		MPI_Send(&img_size, 1, MPI_INT, 0, 0, a_inter_comm);
		MPI_Send(&img.width, 1, MPI_INT, 0, 0, a_inter_comm);
		MPI_Send(&img.height, 1, MPI_INT, 0, 0, a_inter_comm);
		MPI_Send(img.pixels, img_size, MPI_INT, 0, 0, a_inter_comm);

		MPI_Send(&pixels_per_proc, 1, MPI_INT, 0, 0, b_inter_comm);
		MPI_Send(&c_limits[2], 2, MPI_INT, 0, 0, b_inter_comm);
		MPI_Send(&img_size, 1, MPI_INT, 0, 0, b_inter_comm);
		MPI_Send(&img.width, 1, MPI_INT, 0, 0, b_inter_comm);
		MPI_Send(&img.height, 1, MPI_INT, 0, 0, b_inter_comm);
		MPI_Send(img.pixels, img_size, MPI_INT, 0, 0, b_inter_comm);

		printf("i will recv %d ints\n", c_limits[1] - c_limits[0] + 1);
		printf("i will recv %d ints\n", c_limits[3] - c_limits[2] + 1);

		MPI_Status status;
		MPI_Recv(&pixels[c_limits[0]], c_limits[1] - c_limits[0] + 1, MPI_INT, 0, 0, a_inter_comm, &status);
		MPI_Recv(&pixels[c_limits[2]], c_limits[3] - c_limits[2] + 1, MPI_INT, 0, 0, b_inter_comm, &status);

		printf("recv\n");
	}

	int i;
	for (i = limits[0]; i <= limits[1]; i++)
		image_set_pixel(&img, pixels[i], i%img.width, i/img.width);

	toc = tms();

	time_one_thread = (toc - tic) / (float)1000;
	printf("Time: %.4f\n", time_one_thread);
	printf("Saving image..\n");
	save_ppm(argv[2], &img);
	printf("Done\n");
	image_free(&img);
	
	MPI_Finalize();
	return 0;
}
