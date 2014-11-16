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
	int tn;
	int tm;
	int i;

	unsigned long tic;
	unsigned long toc;

	float time_one_thread;
	float time;
	float speedup;
	float efficiency;
	int rank, size;


	image_t img;
	image_new(&img);

//	printf("Loading image.. %s\n", argv[1]);
	load_ppm(argv[1], &img);

	MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
	if (rank == 0) {
		tic = tms();
	}

	image_median_filter(&img);
	if (rank == 0) {
		toc = tms();

		time_one_thread = (toc - tic) / (float)1000;
		printf("Time: %.4f\n", time_one_thread);
		printf("Saving image..\n");
		save_ppm(argv[2], &img);
		printf("Done\n");
	}
	image_free(&img);

	MPI_Finalize();
	return 0;
}
