#include <stdio.h>
#include "structs.h"

#define LEN 3
#define FORSUM(X,L,A) for(int X=1;X<L;X++) A[L] += A[X]

int main(int argc, char * argv[])
{
    printf("Size Test\n");
    
    long s[] =
    {
        sizeof(KalmanFilter),
//        3*sizeof(image_t),
        sizeof(DensityMapPair),
//        sizeof(PeakListPair),
        sizeof(Prediction),
        0
    };
    
    printf("KalmanFilter is %lu bytes\n", s[0]);
//    printf("Image (%d) is %lu bytes\n", IMAGE_DIMENSION, s[1]);
    printf("Density Map Pair (%d) is %lu bytes\n", IMAGE_DIMENSION, s[1]);
//    printf("Peak List Pair (%d) is %lu bytes\n", MAX_PEAKS, s[3]);
    printf("Prediction is %lu bytes\n", s[2]);
    
    FORSUM(i,LEN,s);
    
    printf("Total is %lu bytes or %.3f kbytes\n", s[LEN], ((double)s[LEN])/1000);
    
    return 0;
}
