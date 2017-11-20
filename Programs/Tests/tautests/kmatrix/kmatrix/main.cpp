//
//  main.cpp
//  kmatrix
//
//  Created by Matthew Fonken on 10/9/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <iostream>

#include "global_types.h"
#include "kmat.h"

#define MAX_STATE   14
#define NUM_PEAKS   9
#define LOOPS       100

//double ex_map[MAX_STATE][NUM_PEAKS] = {{1,3,8}, {2,4,7}, {3,4,6}};
//double ex_den[MAX_STATE][NUM_PEAKS] = {{4,2,4}, {4,2,4}, {5,1,5}};
//double ex_map[MAX_STATE][NUM_PEAKS] = {{1,8}, {2,7}, {5,6}, {3,7}};
//double ex_den[MAX_STATE][NUM_PEAKS] = {{4,4}, {4,4}, {5,5}, {4,4}};
//double ex_map[MAX_STATE][NUM_PEAKS] = {{10,100}, {12,111}, {15,99}, {13,90}, {19,80}, {20,70}, {30,80}, {40,89}, {30,85}, {20,93} };
//double ex_den[MAX_STATE][NUM_PEAKS] = {{4,4}, {4,4}, {5,5}, {4,4}, {4,4}, {4,4}, {5,5}, {4,4}, {4,4}, {4,4} };
//double ex_map[MAX_STATE][NUM_PEAKS] = {{1,3,8}, {2,4,7}, {3,4,6}, {4,3,5}, {5,3,4}, {6,4,3}, {7,4,2}, {8,3,1}, {7,4,2}, {6,4,3}, {5,3,4}, {4,3,5}, {3,4,6}, {2,4,7} };
//double ex_den[MAX_STATE][NUM_PEAKS] = {{4,2,4}, {4,2,4}, {5,1,5}, {4,2,4}, {4,2,4}, {5,1,5}, {4,2,4}, {4,2,4}, {5,1,5}, {4,2,4}, {4,2,4}, {5,1,5}, {4,2,4}, {4,2,4} };

double ex_map[MAX_STATE][NUM_PEAKS] = {{1,3,8,100,200,300,400,500,600}, {2,4,7,100,200,300,400,500,600}, {3,4,6,100,200,300,400,500,600}, {4,3,5,100,200,300,400,500,600}, {5,3,4,100,200,300,400,500,600}, {6,4,3,100,200,300,400,500,600}, {7,4,2,100,200,300,400,500,600}, {8,3,100,200,300,400,500,600,1}, {7,4,2,100,200,300,400,500,600}, {6,4,3,100,200,300,400,500,600}, {5,3,4,100,200,300,400,500,600}, {4,3,5,100,200,300,400,500,600}, {3,4,6,100,200,300,400,500,600}, {2,4,7,100,200,300,400,500,600} };
double ex_den[MAX_STATE][NUM_PEAKS] = {{4,2,4,1,1,1,1,1,1}, {4,2,4,1,1,1,1,1,1}, {5,1,5,1,1,1,1,1,1}, {4,2,4,1,1,1,1,1,1}, {4,2,4,1,1,1,1,1,1}, {5,1,5,1,1,1,1,1,1}, {4,2,4,1,1,1,1,1,1}, {4,2,4,1,1,1,1,1,1}, {5,1,5,1,1,1,1,1,1}, {4,2,4,1,1,1,1,1,1}, {4,2,4,1,1,1,1,1,1}, {5,1,5,1,1,1,1,1,1}, {4,2,4,1,1,1,1,1,1}, {4,2,4,1,1,1,1,1,1} };

int global_state = 0;

int getMap(int i)
{
    return ex_map[global_state][i];
}

int getDensity(int i)
{
    return ex_den[global_state][i];
}

void initPeakList( peak_list_t * p, int len)
{
    p->map = (uint16_t*)malloc(sizeof(uint16_t)*len);
    p->density = (uint16_t*)malloc(sizeof(uint16_t)*len);
    p->direction = (int*)malloc(sizeof(int)*len);
    p->length = 0;
}

void generatePeakList(peak_list_t * p, int len)
{
    for(int i = 0; i < len; i++)
    {
        p->map[i]       = getMap(i);
        p->density[i]   = getDensity(i);
        p->direction[i] = 0;
    }
    p->length = len;
}

int main(int argc, const char * argv[])
{
    printf("Starting...\n");
    
    printf("Initializing Kalman Matrix\n");
    kmat_t m;
    initKmat( &m );
    
    printf("Generating Peaks List...\n");
    peak_list_t p;
    initPeakList(&p, NUM_PEAKS);
    generatePeakList(&p, NUM_PEAKS);
    
    printf("Updating Kalman Matrix\n");
    int v = 0;
    for(int i = 0; i < LOOPS-1; i++)
    {
        for(; global_state < MAX_STATE; global_state++)
        {
//            printf("Interation #%d:\n",global_state);
            generatePeakList(&p, NUM_PEAKS);
            updateKmat( &m, &p );
            std::cin.get();
        }
        global_state = 0;
    }
    v = updateKmat( &m, &p );

    int l = m.k_index;
    printf("Kalman Matrix values(%d): ",l);
    for(int i = 0, il = m.lookup[0]; i < l; i++, il = m.lookup[i])
    {
        int c = m.selection[il];
        int s = (int)(m.kalmans[il][c].value);
        printf("[%d]-%d(%d) ", il, s,c);
        
    }
    printf("\n");
    printf("Return is %d\n", v);
    
    return 0;
}
