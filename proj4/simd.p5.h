#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>
#include <sys/time.h>
#include <sys/resource.h>
#include <xmmintrin.h>

#include <omp.h>


#ifndef NUMK
#define NUMK	                4500000    
#endif

#ifndef NUMTRIES
#define NUMTRIES	            1000
#endif

#ifndef NUMT
#define NUMT                    4
#endif

#ifndef NUM_ELEMENTS_PER_CORE   
#define NUM_ELEMENTS_PER_CORE   NUMK/NUMT
#endif

#ifndef SIMD_H
#define SIMD_H
// SSE stands for Streaming SIMD Extensions

#define SSE_WIDTH	4

#define ALIGNED		__attribute__((aligned(16)))

float A[NUMK];
float B[NUMK];
float C[NUMK];

void	SimdMul(    float *, float *,  float *, int );
void	NonSimdMul( float *, float *,  float *, int );

#endif		// SIMD_H