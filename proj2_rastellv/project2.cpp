#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// setting the number of threads:
#ifndef NUMT
#define NUMT		    1
#endif

#ifndef NUMNODES
#define NUMNODES	10
#endif

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES	10
#endif

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

#define N	0.70

float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float r = 1. - xn - yn;
	if( r <= 0. )
	        return 0.;
	float height = pow( r, 1./(float)N );
	return height;
}


int main( int argc, char *argv[ ] )
{
	omp_set_num_threads( NUMT ); // set the number of threads to use in parallelizing the for-loop:`

	double maxPerformance = 0.;	// must be declared outside the NUMTRIES loop
	// the area of a single full-sized tile:
	// (not all tiles are full-sized, though)

	float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );
	
	float halfTileArea = fullTileArea/2;
	float quarterTileArea = fullTileArea/4;
	double volume;

	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for loop and a reduction:

	// looking for the maximum performance:
	for( int tries = 0; tries < NUMTRIES; tries++ )
	{
		double time0 = omp_get_wtime( );
		volume = 0;


		#pragma omp parallel for collapse(2) default(none), shared(fullTileArea, halfTileArea, quarterTileArea), reduction(+:volume)
		for( int iv = 0; iv < NUMNODES; iv++ )
		{
			for( int iu = 0; iu < NUMNODES; iu++ )
			{
				float z = Height( iu, iv );

				if (iv == 0) 
				{
					if ((iu == 0) || (iu == NUMNODES - 1))
						volume += (z * quarterTileArea);
					else 
						volume += (z * halfTileArea);
				}
				else if (iu == 0)
				{
					if ((iv == 0) || (iv == NUMNODES - 1))
						volume += (z * quarterTileArea);
					else 
						volume += (z * halfTileArea);
				}
				else
					volume += (z * fullTileArea);
			}
		}	

		double time1 = omp_get_wtime( );
		double megaHeightsComputedPerSecond = (double)(NUMNODES*NUMNODES) / ( time1 - time0 ) / 1000000.;
		if( megaHeightsComputedPerSecond > maxPerformance )
			maxPerformance = megaHeightsComputedPerSecond;
	}

	volume *= 2;
	fprintf(stderr, "%2d threads : %5d nodes ; volume = %3.12lf ; megaHeightsComputed/sec = %6.2lf\n",
	NUMT, NUMNODES, volume, maxPerformance);

	return 0;
}
