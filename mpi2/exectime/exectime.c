/*
 * exectime.c
 * Ramon Saraiva
 * 
 */

#include "exectime/exectime.h"

#include <string.h>
#include <time.h>
#include <stdio.h>

void exectime_init(exectime_t* exectime)
{
	memset(exectime, 0, sizeof(exectime_t));
}

void exectime_start(exectime_t* exectime)
{
	exectime->tic = clock();
}

void exectime_stop(exectime_t* exectime)
{
	exectime->toc = clock();
	float t = exectime->toc - exectime->tic;
	exectime->time += t / CLOCKS_PER_SEC;
}

void exectime_reset(exectime_t* exectime)
{
	exectime->time = 0;
}

float exectime_result_nano(exectime_t* exectime)
{
	return exectime->time * 1000000000;
}

float exectime_result_micro(exectime_t* exectime)
{
	return exectime->time * 1000000;
}

float exectime_result_secs(exectime_t* exectime)
{
	return exectime->time;
}
