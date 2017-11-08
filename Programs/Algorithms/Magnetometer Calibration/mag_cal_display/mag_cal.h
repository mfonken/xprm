//
//  mag_cal.h
//  
//
//  Created by Matthew Fonken on 12/21/16.
//
//

#ifndef mag_cal_h
#define mag_cal_h

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define MAX_SAMPLES     5000

/* See - AN4246: Calibrating an eCompass in the Presence of Hard and Soft Iron Interference */

typedef struct
{
    uint16_t n;                  /**< Number of sample */
    long double s[MAX_SAMPLES][3];  /**< Samples */
} mag_cal_t;


/*For calculating Determinant of the Matrix */
long double determinant(long double a[4][4], long double k)
{
    long double s = 1, det = 0, b[4][4];
    int i, j, m, n, c;
    if( k == 1 )
    {
        return( a[0][0] );
    }
    else
    {
        det = 0;
        for( c = 0; c < k; c++ )
        {
            m = 0;
            n = 0;
            for( i = 0; i < k; i++ )
            {
                for( j = 0; j < k; j++ )
                {
                    b[i][j] = 0;
                    if( i != 0 && j != c )
                    {
                        b[m][n] = a[i][j];
                        if( n < ( k - 2 ) )
                            n++;
                        else
                        {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
            det = det + s * ( a[0][c] * determinant( b, ( k - 1 ) ) );
            s *= -1;
        }
    }
    return det;
}

void invert( long double *matrix[4][4] )
{
    
}

    

/* Eq. 27: Error residual vector r = Y - Xß */
/* Eq. 34: Ordinary Least Squares Solution vector ß = (X^T*X)^-1 * X^T*Y = [2Vx; 2Vy; 2Vz; B^2-Vx^2-Vy^2-Vz^2] */
/* Returns hard-iron offset[0-2] and geomagnetic field strength B[3] */
void performCalibration( long double V[4], mag_cal_t * samples )
{
    uint16_t n = samples->n;
    if( n > MAX_SAMPLES )
        n = MAX_SAMPLES;
    
    /* Vector Y - Known dependent variables */
    long double Y[n];
    
    for(int i = 0; i < n; i++ )
    {
        long double Bpx = samples->s[i][0];
        long double Bpy = samples->s[i][1];
        long double Bpz = samples->s[i][2];
        Y[i] = ( Bpx * Bpx ) + ( Bpy * Bpy ) + ( Bpz * Bpz );
//        printf("(%f,%f,%f)>%d:%f\n", Bpx, Bpy, Bpz, i, Y[i]);
    }
    
    /* Transposed Measurement Matrix X */
    long double X_T[4][n];
    for( int i = 0; i < n; i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            
            X_T[j][i] = samples->s[i][j];
        }
        X_T[3][i] = 1;
    }
    
    long double X_T_X[4][4];
    for( int j = 0; j < 4; j++ )
    {
        for( int i = 0; i < 4; i++ )
        {
            long double *row;
            row = X_T[i];
            long double product = 0;
            for( int k = 0; k < n; k++ )
            {
                product += row[k] * X_T[j][k];
            }
            X_T_X[j][i] = product;
        }
    }
    
    long double d =  determinant(X_T_X,4);
    
    /* Calculate Inverse */
    long double b[4][4], c[4][4];
    long double X_T_X_INV[4][4];
    int u,v;
    for( int q = 0; q < 4 ; q++ )
    {
        for( int p = 0 ; p < 4 ; p++ )
        {
            u = 0;
            v = 0;
            for( int i = 0; i < 4 ; i++ )
            {
                for( int j = 0; j < 4; j++ )
                {
                    if( i != q && j != p )
                    {
                        b[u][v] = X_T_X[i][j];
                        if( v < 2)
                        {
                            v++;
                        }
                        else
                        {
                            v = 0;
                            u++;
                        }
                    }
                }
            }
            c[q][p] = pow( -1, q + p ) * determinant(b,3);
            X_T_X_INV[p][q] = c[q][p] / d;
        }
        
    }
    
    
    
    long double X_T_Y[4];
    for( int j = 0; j < 4; j++ )
    {
        long double product = 0;
        for( int k = 0; k < n; k++ )
        {
            product += X_T[j][k] * Y[k];
        }
        X_T_Y[j] = product;
    }
    
    long double beta[4];
    for( int j = 0; j < 4; j++ )
    {
        long double product = 0;
        for( int k = 0; k < 4; k++ )
        {
            product += X_T_X_INV[j][k] * X_T_Y[k];
        }
        beta[j] = product;
    }
    
    /* Extract Hard Iron Offset */
    long double HIO[4];
    for(int i = 0; i < 3; i++ )
    {
        HIO[i] = beta[i] / 2;
    }
    HIO[3] = sqrt( beta[3] + ( HIO[0] * HIO[0] ) + ( HIO[1] * HIO[1] ) + ( HIO[2] * HIO[2] ) );
    
    for( int i = 0; i < 4; i++ )
    {
        V[i] = HIO[i];
    }
}


#endif /* mag_cal_h */
