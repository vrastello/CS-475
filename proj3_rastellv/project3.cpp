#include <stdio.h>
unsigned int seed = 0;
#include <math.h>
#include <stdlib.h>
#include <omp.h>

float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

float x = Ranf( &seed, -1.f, 1.f );

int	NowYear;		// 2021 - 2026
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	    NowNumDeer;		// number of deer in the current population
int     NowNumWardens;  // number of Worrisome Wardens in population

const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

float
SQR( float x )
{
        return x*x;
}

void Worrisome_Wardens(void) 
{
    while( NowMonth < 73 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        int nextNumWardens = NowNumWardens;
        if( (NowNumDeer > 5) || (NowHeight > 5) )
                nextNumWardens++;
        else
            {
                nextNumWardens--;
                if(nextNumWardens < 0.)nextNumWardens = 0.;
            }
        // DoneComputing barrier:
        #pragma omp barrier

        NowNumWardens = nextNumWardens;

        // DoneAssigning barrier:
        #pragma omp barrier

        // DonePrinting barrier:
        #pragma omp barrier
    }
    return;
}

void Deer(void) 
{
    while( NowMonth < 73 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        int nextNumDeer = NowNumDeer;
        int carryingCapacity = (int)( 3*NowHeight );
        int wardenDeerEffect = nextNumDeer - (NowNumWardens*2);
        if((NowNumWardens > 0)&&(nextNumDeer >5))
        {
            if(wardenDeerEffect >= 5)
                nextNumDeer -= (NowNumWardens*2);
            else
                nextNumDeer = 5;
        }

        if( nextNumDeer < carryingCapacity )
            nextNumDeer += 5;
        else
            if( nextNumDeer > carryingCapacity )
                nextNumDeer--;

            if( nextNumDeer < 0 )
                nextNumDeer = 0;

        // DoneComputing barrier:
        #pragma omp barrier

        NowNumDeer = nextNumDeer;

        // DoneAssigning barrier:
        #pragma omp barrier

        // DonePrinting barrier:
        #pragma omp barrier
    }
    return;
}

void Grain(void)
{

    while( NowMonth < 73 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
        float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

        float nextHeight = NowHeight;
        nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
        float wardenGrainEffect = nextHeight - NowNumWardens;
        if((NowNumWardens > 0)&&(nextHeight >5))
        {
            if(wardenGrainEffect >= 5)
                nextHeight -= NowNumWardens;
            else
                nextHeight = 5;
        }

        if( nextHeight < 0. ) nextHeight = 0.;

        // DoneComputing barrier:
        #pragma omp barrier
        NowHeight = nextHeight;

        // DoneAssigning barrier:
        #pragma omp barrier

        // DonePrinting barrier:
        #pragma omp barrier
    }
    return;
}

void Watcher(void)
{
    while( NowMonth < 73 )
    {   
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:

        #pragma omp barrier

        // DoneAssigning barrier:
        #pragma omp barrier
        
        //Print current Globals 
        float heightCm = NowHeight * 2.54;
        float cm = NowPrecip * 2.54;
        float celcius = (5./9.)*(NowTemp-32);
        fprintf(stderr, "%2d,%6.2f,%6.2f,%6.2f,%2d,%2d\n",
	    NowMonth, cm, celcius, heightCm, NowNumDeer, NowNumWardens);

        //increment time
        NowMonth++;
        //if(NowMonth > 11)
        //{
            //NowMonth = 0;
           // NowYear++;
       // }

        //update NowTemp and NowPrecip
        float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

        float temp = AVG_TEMP - AMP_TEMP * cos( ang );
        NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
        NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
        if( NowPrecip < 0. )
            NowPrecip = 0.;

        // DonePrinting barrier:
        #pragma omp barrier
    }
    return;
}

int main()
{
    omp_set_num_threads( 4 );	// same as # of sections
    // starting date and time:
    NowMonth =    0;
    NowYear  = 2021;

    // starting state 
    NowNumDeer = 1;
    NowHeight =  1.;
    NowNumWardens = 0;

    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
    if( NowPrecip < 0. )
        NowPrecip = 0.;

    #pragma omp parallel sections 
    {
        #pragma omp section
        {
            Deer();
        }

        #pragma omp section
        {
            Grain();
        }

        #pragma omp section
        {
            Watcher();
        }

        #pragma omp section
        {
            Worrisome_Wardens();	
        }
    }       // implied barrier -- all functions must return in order
        // to allow any of them to get past here


    return 0;

}