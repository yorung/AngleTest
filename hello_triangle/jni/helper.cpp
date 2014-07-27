#include "stdafx.h"

#ifdef _MSC_VER
double GetTime()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double)t.QuadPart / f.QuadPart;
}
#else
double GetTime()
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec + (double)t.tv_nsec / 1000000000;
}
#endif

float Random()
{
	return (float)rand() / RAND_MAX;
}
