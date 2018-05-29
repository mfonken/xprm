#include <stdio.h>
#include <stdint.h>
#include "general.hpp"
#define W 99
#define H 99

#define ZDIV(a,b) (!b?2<<7:a/b)

centroids_t centroid = { 25, 50, 33, 66 };
int current[4] = { 40, 20, 20, 100 }, background[4] = { 30, 20, 20, 10 };

density_redistribution_lookup_t lookup =
{
    {
        {
            { { 0, 1, 3, 4 }, { 2, 5 }, { 6, 7 }, { 8 } },
            { { 0 }, { 1, 2 }, { 3, 6 }, { 4, 5, 7, 8 } },
            { { 0, 1, 2, 3 }, { 1, 3 }, { 2, 3 }, { 3 } },
            { 4, 2, 2, 1 }
        },
        {
            { { 0, 3 }, { 1, 2, 4, 5 }, { 6 }, { 7, 8 } },
            { { 0, 1 }, { 2 }, { 3, 4, 6, 7 }, { 5, 8 } },
            { { 0, 2 }, { 0, 1, 2, 3 }, { 2 }, { 2, 3 } },
            { 2, 4, 1, 2 }
        },
        {
            { { 0, 1 }, { 2 }, { 3, 4, 6, 7 }, { 5, 8 } },
            { { 0, 3 }, { 1, 2, 4, 5 }, { 6 }, { 7, 8 } },
            { { 0, 1 }, { 1 }, { 0, 1, 2, 3 }, { 1, 3 } },
            { 2, 1, 4, 2 }
        },
        {
            { { 0 }, { 1, 2 }, { 3, 6 }, { 4, 5, 7, 8 } },
            { { 0, 1, 3, 4 }, { 2, 5 }, { 6, 7 }, { 8 } },
            { { 0 }, { 0, 1 }, { 0, 2 }, { 0, 1, 2, 3 } },
            { 1, 2, 2, 4 }
        }
    }
};

void Redistribute( centroids_t * C, int Q[4], int Qb[4] )
{
    int c = ( (int)( C->Cy < C->By ) << 1 ) + (int)( C->Cx < C->Bx );
    for( int q = 0; q < 4; q++ )
    {
        
        int bd = 0, l = lookup.config[c].length[q], l_ = lookup.config[c].length[3-q];
        printf("q=%d : l=%d|%d\n", q, l, l_);
        for( int i = 0; i < l; i++ )
        {
            int a = 0, b = 0, aa, ab, ac, ba, bb, bc;
            aa = lookup.config[c].current[q][i];
            ab = lookup.area[aa];
            a = ab * Qb[q];
            
            b = 0;
            for( int j = 0; j < l_; j++ )
            {
                int fi = lookup.config[c].factor[q][i];
                ba = lookup.config[c].background[fi][j];
                b += lookup.area[ba];
            }
            if(!b)
                b = 1;
            bd += ZDIV( a, b );
            
            printf("\ta=%d/b=%d | bd=%d\n", a,b,bd);
        }
        if( bd > Q[q] ) Q[q] = 0;
        else Q[q] -= bd;
    }
}

void generate_areas( centroids_t * C )
{
    int x0, x1, y0, y1;
    if( C->Cx < C->Bx ) { x0 = C->Cx; x1 = C->Bx;}
    else { x0 = C->Bx; x1 = C->Cx;}
    if( C->Cy < C->By ) { y0 = C->Cy; y1 = C->By;}
    else { y0 = C->By; y1 = C->Cy;}
    
    int xl[3] = {x0, x1-x0, W-x1},
    yl[3] = {y0, y1-y0, H-y1};
    for( int y = 0, p = 0; y < 3; y++ )
        for( int x = 0; x < 3; x++ )
            lookup.area[p++] = xl[x]* yl[y];
}

void print_lookup()
{
    for(int c = 0; c < 4; c++)
    {
        printf("Config %d\n\t Background { ", c);
        for(int q = 0; q < 4; q++)
            printf("{%d,%d,%d,%d} ", lookup.config[c].background[q][0], lookup.config[c].background[q][1], lookup.config[c].background[q][2], lookup.config[c].background[q][3]);
        printf("}\n\t Current { ");
        for(int q = 0; q < 4; q++)
            printf("{%d,%d,%d,%d} ", lookup.config[c].current[q][0], lookup.config[c].current[q][1], lookup.config[c].current[q][2], lookup.config[c].current[q][3]);
        printf("}\n\t Factor { ");
        for(int q = 0; q < 4; q++)
            printf("{%d,%d,%d,%d} ", lookup.config[c].factor[q][0], lookup.config[c].factor[q][1], lookup.config[c].factor[q][2], lookup.config[c].factor[q][3]);
        printf("}\n\t Lengths");
        printf("{%d,%d,%d,%d} ", lookup.config[c].length[0], lookup.config[c].length[1], lookup.config[c].length[2], lookup.config[c].length[3]);
        printf("}\n");
    }
    printf("Areas { ");
    for(int i = 0; i < 8; i++)
    {
        printf("%d, ", lookup.area[i]);
    }
    printf("%d }\n", lookup.area[8]);
}

void print_quadrants()
{
    printf("Current:    { ");
    for( int i = 0; i < 4; i++ ) printf("%d ", current[i]);
    printf("}\nBackground: { ");
    for( int i = 0; i < 4; i++ ) printf("%d ", background[i]);
    printf("}\n");
}

int main(int argc, char * argv[] )
{
    print_quadrants();
    generate_areas(&centroid);
    print_lookup();
    Redistribute( &centroid, current, background );
    print_quadrants();
    
    return 0;
}

