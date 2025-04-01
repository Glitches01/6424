/* ***********************************************
* receiver .c
* ----------------------------------------------
* Flush + Reload covert channel receiver ( instruction page )
* Loops indefinitely , decoding bits until killed .
*********************************************** */
# define _GNU_SOURCE
# include <stdio.h>
# include <stdint.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <x86intrin.h>
# include <math.h>
# include <time.h>

// A threshold cycles for deciding if it ’s a cache hit or miss
// You MUST TUNE THIS for your CPU . Start with 100 cycles
# define THRESHOLD_CYCLES 1000
// Read timestamp counter
static inline uint64_t rdtscp64 () {
    unsigned aux ;
    return __rdtscp (& aux ) ;
}

// Read timestamp counter

int main (int argc , char * argv []) {

    int threshold = THRESHOLD_CYCLES ;

    for (int i = 1; i < argc ; i ++) {
        if ( strcmp ( argv [ i ] , "-t") == 0 && ( i + 1) < argc ) {
            threshold = atoi ( argv [++ i ]) ;
        }
    }
    printf ("[ Receiver ] Starting up ...\n") ;
    fflush ( stdout ) ;

    double pi = 3.141592653589793;
    int decpt = 0 , sign = 0;
    char buf [64];
    uint64_t start , end ;
    int bitcnt = 0 , val = 0;

    // while (1) 
    for (int i = 0; i < 10000; i++)
    {
        start = rdtscp64 () ;
        // int result = atoi ("1234567890") ;
        int s = ecvt_r ( pi , 20 , & decpt , & sign , buf , sizeof ( buf ) ) ;
        end = rdtscp64 () ;
        uint64_t t = end - start ;
        printf("t: %d buf %s \n",t,buf);
        int bit = (t < threshold) ? 1 : 0;
        printf ("%d\n", bit) ;
        fflush (stdout);
    }
    // {
    //     start = rdtscp64 () ;
    //     // int result = atoi ("1234567890") ;
    //     int s = ecvt_r ( pi , 20 , & decpt , & sign , buf , sizeof ( buf ) ) ;
    //     end = rdtscp64 () ;
    //     uint64_t t = end - start ;
    //     printf("t: %d buf %s",t,buf);
    //     int bit = ( t < threshold ) ? 1 : 0;
    //     printf ("%d \n", bit ) ;
    //     fflush ( stdout ) ;
    // }

    return 0;
}