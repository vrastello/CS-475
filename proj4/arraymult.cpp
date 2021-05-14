#include "simd.p5.h"

void
NonSimdMul( float *a, float *b,   float *c,   int len )
{

	for( int i = 0; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}


int main( int argc, char *argv[ ] ) {

    	// inialize the arrays:
	for( int i = 0; i < NUMK; i++ )
	{
		A[ i ] = 1.;
		B[ i ] = 2.;
	}

    double maxPerformance = 0.;	// must be declared outside the NUMTRIES loop

    // looking for the maximum performance:
    for( int tries = 0; tries < NUMTRIES; tries++ )
    {
        double time0 = omp_get_wtime( );


         NonSimdMul(A, B, C, NUMK);


        double time1 = omp_get_wtime( );
		double megaMultsPerSecond = (double)NUMK / ( time1 - time0 ) / 1000000.;
		if( megaMultsPerSecond > maxPerformance )
			maxPerformance = megaMultsPerSecond;

    }

    fprintf(stderr, "nonsimd, %10d, %8.2lf\n",
    NUMK, maxPerformance);

	return 0;
}