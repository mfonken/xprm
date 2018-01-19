//
//  main.c
//  
//
//  Created by Matthew Fonken on 1/2/18.
//

#include <stdio.h>
#include <stdlib.h>
#include "fisheye_math.h"

#define W 400
#define H 400

int main(int argc, char * argv[])
{
    ps();
    point2_t A = {20,20}, B, C;
    if(argc > 2)
    {
        A.x = atoi(argv[1]);
        A.y = atoi(argv[2]);
    }
    pp2("Testing:", &A);

    unfisheye(&A, &B, W, H);
    
    printf("\nAfter inverse fisheye:\n");
    pp2("A:", &A);
    pp2("B:", &B);
    
    fisheye(&B, &C, W, H);
    
    printf("\nAfter fisheye:\n");
    pp2("B:", &B);
    pp2("C:", &C);
    
    return 0;
}
