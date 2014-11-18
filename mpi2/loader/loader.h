#ifndef LOADER_H
#define	LOADER_H	1

#include "image/image.h"

int load_ppm(char* p, image_t* i);
int save_ppm(char* p, image_t* img);

#endif
