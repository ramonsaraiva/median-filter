/*
 * exectime.h
 * Ramon Saraiva
 *
 */

#ifndef EXECTIME_H
#define EXECTIME_H	1

#include <time.h>

struct exectime_s
{
	clock_t tic;
	clock_t toc;
	float time;
};

typedef struct exectime_s exectime_t;

void exectime_init(exectime_t* exectime);
void exectime_start(exectime_t* exectime);
void exectime_stop(exectime_t* exectime);
void exectime_reset(exectime_t* exectime);
float exectime_result_nano(exectime_t* exectime);
float exectime_result_micro(exectime_t* exectime);
float exectime_result_secs(exectime_t* exectime);

#endif
