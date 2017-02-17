//
//  euler.c
//  
//
//  Created by Matthew Fonken on 1/26/17.
//
//

#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#define PI      M_PI//3.141596
#define RAD_TO_DEG  180 / PI

#define PHI_P   PI/100
#define THT_P   PI/50
#define PSI_P   PI/20

#define PHI_C   PI/20
#define THT_C   PI/40
#define PSI_C   PI/80

#define PHI_B   PI/1000
#define PSI_B   -PI/1000

#define D_FIXED 1

//#define DEBUG

void multiplyVec3x1( double a[3][3], double b[3], double c[3] )
{
    double r[3] = {0,0,0};
    for( int i = 0; i < 3; i++ )
    {
        r[i] = a[i][0] * b[0] + a[i][1] * b[1] + a[i][2] * b[2];
    }
    for( int i = 0; i < 3; i++ )
    {
        c[i] = r[i];
    }
}

void multiplyVec3x3( double a[3][3], double b[3][3], double c[3][3] )
{
    double r[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            for( int k = 0; k < 3; k++ )
            {
                r[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            c[i][j] = r[i][j];
        }
    }
}


void getRotationX( double v[3][3], double angle )
{
    double c = cos( angle );
    double s = sin( angle );
    double r[3][3] =
    {
        {  1,  0,  0 },
        {  0,  c, -s },
        {  0,  s,  c }
    };
    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            v[i][j] = r[i][j];
        }
    }
}

void getRotationY( double v[3][3], double angle )
{
    double c = cos( angle );
    double s = sin( angle );
    double r[3][3] =
    {
        {  c,  0,  s },
        {  0,  1,  0 },
        { -s,  0,  c }
    };
    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            v[i][j] = r[i][j];
        }
    }
}

void getRotationZ( double v[3][3], double angle )
{
    double c = cos( angle );
    double s = sin( angle );
    double r[3][3] =
    {
        {  c, -s,  0 },
        {  s,  c,  0 },
        {  0,  0,  1 }
    };
    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            v[i][j] = r[i][j];
        }
    }
}

void printRotation( double r[3][3], char c )
{
#ifdef DEBUG
    printf("%c - \n\t", c);
    for( int i = 0; i < 3; i++ )
    {
        printf("( %.3f, %.3f, %.3f )\n\t", r[i][0], r[i][1], r[i][2] );
    }
    printf("\n");
#endif
}

void printVector( double v[3] )
{
    printf("v - \n\t");
    printf("( %.3f, %.3f, %.3f )\n\t", v[0], v[1], v[2]);
    printf("\n");
}

int main(int argc, char * args[])
{
    clock_t t1, t2;
    double diff, ret;
    t1 = clock();
    
    double  z_p[3][3],
            y_p[3][3],
            x_p[3][3],
            z_c[3][3],
            y_c[3][3],
            x_c[3][3],
            z_b[3][3],
            x_b[3][3];
    
    getRotationZ( z_p, PSI_P );
    getRotationY( y_p, THT_P );
    getRotationX( x_p, PHI_P );
    getRotationZ( z_c, PSI_C );
    getRotationY( y_c, THT_C );
    getRotationX( x_c, PHI_C );
    getRotationZ( z_b, PSI_B );
    getRotationX( x_b, PHI_B );
    double r_p[3][3];
    multiplyVec3x3( z_p, y_p, r_p );
    multiplyVec3x3( r_p, x_p, r_p );
    double r_c[3][3];
    multiplyVec3x3( z_c, y_c, r_c );
    multiplyVec3x3( r_c, x_c, r_c );
    double r_b[3][3];
    multiplyVec3x3( z_b, x_b, r_b );
    double r_a[3][3];
    multiplyVec3x3( r_p, r_c, r_a );
    multiplyVec3x3( r_a, r_b, r_a );
    double alpha = acos( r_b[1][1] ); //acos( cos( PSI_B ) * cos( THT_B ) );
    double mu = acos( r_a[2][2] );
    double q = cos( mu - alpha ) / sin( alpha ) * D_FIXED;
    double v[3] = {q, 0, 0};
    double o[3];
    multiplyVec3x1( r_a, v, o );
    t2 = clock();
    
    printRotation( r_p, 'p' );
    printRotation( r_c, 'c' );
    printRotation( r_b, 'b' );
    printRotation( r_a, 'a' );
    printf("alpha - %.3fº\n", alpha * RAD_TO_DEG);
    printf("   mu - %.3fº\n", mu * RAD_TO_DEG);
    printf("sigma - %f\n", q);
    printVector( o );
    
    diff = ((double)(t2 - t1) / 1000000.0F ) * 1000;
    printf("Matrix Rotation - %.3fms\n", diff);
    return 0;
}
