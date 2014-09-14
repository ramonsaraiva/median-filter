#include <stdio.h>
#include "image/image.h"
#include "loader/loader.h"

int main(int argc, char** argv)
{
	image_t img;
	image_new(&img);

	load_ppm("a.ppm", &img);
	save_ppm("b.ppm", &img);

	return 0;
}
