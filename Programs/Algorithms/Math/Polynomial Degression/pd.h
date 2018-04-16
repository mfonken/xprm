/*! \file pd.h
    \brief Polynomial Degression
 
    Created by Matthew Fonken on 10/15/16.
*/

#ifndef pd_h
#define pd_h

#include <math.h>
#include <stdint.h>
#include <sys/time.h>
#include <iostream>

#ifndef PI
#define PI 3.14159
#endif

#define DEBUG

#define MAX_PATTERN_VARIANCE    1.0 /**< Maximum pattern variance */

enum motion_pattern
{
    NONE = 0,
    LINE,
    CIRCLE,
};

/*! Simple 3d Vector */
typedef struct
{
    double    ihat;
    double    jhat;
    double    khat;
} vec3;

typedef struct
{
    double x;
    double y;
    double z;
} cartesian3;

void subtractVec3s( vec3 * x,
                   vec3 * y )
{
    x->ihat = x->ihat - y->ihat;
    x->jhat = x->jhat - y->jhat;
    x->khat = x->khat - y->khat;
}

double lengthOfVec3( vec3 * vec )
{
    return sqrt( ( vec->ihat * vec->ihat ) + ( vec->jhat * vec->jhat ) + ( vec->khat * vec->khat ) );
}

void normalizeVec3( vec3 * vec )
{
    double length = lengthOfVec3( vec );
    vec->ihat /= length;
    vec->jhat /= length;
    vec->khat /= length;
}


/*! Minimum of three numbers */
double minTriple( double a,
                  double b,
                  double c )
{
    if( a < b )
    {
        if( a < c ) return a;
    }
    else
    {
        if( b < c ) return b;
    }
    return c;
}

double linearRegression3d( cartesian3 * samples,
                           uint8_t num_of_samples,
                           cartesian3 * M,
                           cartesian3 * H )
{
    double X_m      = 0;
    double Y_m      = 0;
    double Z_m      = 0;

    double X_m_X_m  = 0;
    double Y_m_Y_m  = 0;
    double Z_m_Z_m  = 0;
    
    double X_m_Y_m  = 0;
    double X_m_Z_m  = 0;
    double Y_m_Z_m  = 0;

#ifdef DEBUG
    printf("*********************REGRESSION PROCEDURE*********************\n");
    printf("Received %d coordinates.\n", num_of_samples);
    
    for(int i = 0; i < num_of_samples; i++)
    {
        printf(" %d. ( %.3f, %.3f, %.3f )\n", i+1, samples[i].x, samples[i].y, samples[i].z);
    }
#endif
    
    for( int k = 0; k < num_of_samples; k++ )
    {
        double x = samples[k].x;
        double y = samples[k].y;
        double z = samples[k].z;
        
        X_m += x;
        Y_m += y;
        Z_m += z;
        
        X_m_X_m += ( x * x );
        Y_m_Y_m += ( y * y );
        Z_m_Z_m += ( z * z );
        
        X_m_Y_m += ( x * y );
        X_m_Z_m += ( x * z );
        Y_m_Z_m += ( y * z );
    }
    
    X_m /= num_of_samples;
    Y_m /= num_of_samples;
    Z_m /= num_of_samples;
    
    X_m_X_m /= num_of_samples;
    Y_m_Y_m /= num_of_samples;
    Z_m_Z_m /= num_of_samples;
    
    X_m_Y_m /= num_of_samples;
    X_m_Z_m /= num_of_samples;
    Y_m_Z_m /= num_of_samples;
  
#ifdef DEBUG
    printf("Means: X|%.3f,   Y|%.3f,   Z|%.3f\n", X_m, Y_m, Z_m);
    printf("      XX|%.3f, YY|%.3f, ZZ|%.3f\n", X_m_X_m, Y_m_Y_m, Z_m_Z_m);
    printf("      XY|%.3f, XZ|%.3f, YZ|%.3f\n", X_m_Y_m, X_m_Z_m, Y_m_Z_m);
    printf("\n");
#endif
    
    double S_x_x    = 0;
    double S_y_y    = 0;
    double S_z_z    = 0;
    double S_x_y    = 0;
    double S_x_z    = 0;
    double S_y_z    = 0;
    
    S_x_x = - ( X_m * X_m ) + X_m_X_m;
    S_y_y = - ( Y_m * Y_m ) + Y_m_Y_m;
    S_z_z = - ( Z_m * Z_m ) + Z_m_Z_m;
    S_x_y = - ( X_m * Y_m ) + X_m_Y_m;
    S_x_z = - ( X_m * Z_m ) + X_m_Z_m;
    S_y_z = - ( Y_m * Z_m ) + Y_m_Z_m;

#ifdef DEBUG
    printf("S:   Sxx|%.3f, Syy|%.3f, Szz|%.3f\n", S_x_x, S_y_y, S_z_z);
    printf("     Sxy|%.3f, Sxz|%.3f, Syz|%.3f\n", S_x_y, S_x_z, S_y_z);
    printf("\n");
#endif
    
    double t = 0.5 * atan2( 2 * S_x_y, S_x_x - S_y_y );

#ifdef DEBUG
    printf("theta: %.3f\n\n", t);
#endif
    
    double cos_a = cos( t );
    double sin_a = sin( t );
    double cos_a_cos_a = cos_a * cos_a;
    double sin_a_sin_a = sin_a * sin_a;
    double S_X_Y_cos_sin = 2 * S_x_y * cos_a * sin_a;
    double K_1_1 = ( S_y_y + S_z_z ) * cos_a_cos_a + ( S_x_x + S_z_z ) * sin_a_sin_a - S_X_Y_cos_sin;
    double K_2_2 = ( S_y_y + S_z_z ) * sin_a_sin_a + ( S_x_x + S_z_z ) * cos_a_cos_a + S_X_Y_cos_sin;
    double K_1_2 = - S_x_y * (cos_a_cos_a - sin_a_sin_a ) + ( S_x_x - S_y_y ) * cos_a * sin_a;
    double K_1_0 =   S_x_z * cos_a + S_y_z * sin_a;
    double K_0_1 = - S_x_z * sin_a + S_y_z * cos_a;
    double K_0_0 =   S_x_x + S_y_y;
 
#ifdef DEBUG
    printf("K:   K00|%.3f, K01|%.3f\n", K_0_0, K_0_1);
    printf("     K11|%.3f, K10|%.3f\n", K_1_1, K_1_0);
    printf("     K22|%.3f, K12|%.3f\n", K_2_2, K_1_2);
    printf("\n");
#endif
    
    double c_0   = ( ( K_0_1 * K_0_1 ) * K_1_1 ) + ( ( K_1_0 * K_1_0 ) * K_2_2 ) - K_0_0 * K_1_1 * K_2_2;
    double c_1   = ( K_0_0 * K_1_1 ) + ( K_0_0 * K_2_2 ) + ( K_1_1 * K_2_2 ) - ( K_0_1 * K_0_1 ) - ( K_1_0 * K_1_0 );
    double c_2   = - K_0_0 - K_1_1 - K_2_2;

#ifdef DEBUG
    printf("c:    c0|%.3f,  c1|%.3f,  c2|%.3f\n", c_0, c_1, c_2);
    printf("\n");
#endif
    
    double c_2_c_2 = ( c_2 * c_2 );
    double c_2_135 = ( c_2_c_2 / 13.5 );
    
#ifdef DEBUG
    printf("c': c2^2|%.3f  c'|%.3f\n", c_2_c_2, c_2_135);
    printf("\n");
#endif
    
    double p     = c_1 - ( c_2_c_2 / 3 );
    double q     = c_2 * ( c_2_135 - ( c_1 / 3 ) ) + c_0;
    double R     = ( ( q / 4 ) * q ) + ( ( ( p * p ) / 27 ) * p );
    double d_m_sq = 0;

#ifdef DEBUG
    printf("p,q,R: p|%.3f,   q|%.3f,   R|%.3f\n", p, q, R);
    printf("\n");
#endif
    
    if( R > 0 )
    {
        double R_sqrt = sqrt( R );
        d_m_sq = - ( 1 / 3 ) * c_2 + cbrt( - 0.5 * q + R_sqrt ) + cbrt( - 0.5 * q - R_sqrt );
    }
    else
    {
        double r = sqrt( - ( ( p * p * p ) / 27 ) );
        
        double f_c = acos( - q / ( 2 * r ) );
        double c_2_c = - ( c_2 / 3 );
        double r_c = 2 * cbrt( r );
     
#ifdef DEBUG
        printf("r': r|%.3f\n", r);
        printf("f,c2c,r: f|%.3f, c2c|%.3f, r|%.3f\n", f_c, c_2_c, r_c);
        printf("\n");
#endif
        
        double d_m_sq_0 = c_2_c + r_c * cos( f_c / 3 );
        double d_m_sq_1 = c_2_c + r_c * cos( ( f_c + 2 * PI ) / 3 );
        double d_m_sq_2 = c_2_c + r_c * cos( ( f_c + 4 * PI ) / 3 );
        
        d_m_sq = minTriple( d_m_sq_0, d_m_sq_1, d_m_sq_2 );
        
#ifdef DEBUG
        printf("d:  d0^2|%.3f, d1^2|%.3f, d2^2|%.3f\n", d_m_sq_0, d_m_sq_1, d_m_sq_2);
        printf("d': d^2|%.3f\n", d_m_sq);
        printf("\n");
#endif
    }
    
    
    
    /* Regression Line Calculation */
    double K_c_0    = ( - K_1_0 / ( K_1_1 - d_m_sq ) );
    double K_c_1    = ( - K_0_1 / ( K_2_2 - d_m_sq ) );
    double a        = K_c_0 * cos_a + K_c_1 * sin_a;
    double b        = K_c_0 * sin_a + K_c_1 * cos_a;
    double H_c      = ( 1 / ( 1 + ( a * a ) + ( b * b ) ) );
#ifdef DEBUG
    printf("Kc':  Kc0|%.3f, Kc1|%.3f\n", K_c_0, K_c_1);
    printf("a,b:  a|%.3f, b|%.3f\n", a, b);
    printf("\n");
#endif
    double u        = H_c * ( ( 1 + ( b * b ) ) * X_m - ( ( a * b ) * Y_m ) + ( a * Z_m ) );
    double v        = H_c * ( ( - ( a * b ) * X_m ) + ( ( 1 + ( a * a ) ) * Y_m ) + ( b * Z_m ) );
    double w        = H_c * ( ( a * X_m ) + ( b * Y_m ) + ( ( ( a * a ) + ( b * b ) ) * Z_m ) );
    
    M->x = X_m;
    M->y = Y_m;
    M->z = Z_m;
    
    H->x = u;
    H->y = v;
    H->z = w;

#ifdef DEBUG
    printf("M:  Mx|%.3f, My|%.3f Mz|%.3f\n", M->x, M->y, M->z);
    printf("H:  Hx|%.3f, Hy|%.3f Hz|%.3f\n", H->x, H->y, H->z);
    printf("\n");
    
    printf("**************************************************************\n");
#endif

    return R;
}

void vec3CrossProduct( vec3 * a,
                       vec3 * b )
{
    a->ihat = ( a->jhat * b->khat ) - ( a->khat * b->jhat );
    a->jhat = ( a->khat * b->ihat ) - ( a->ihat * b->khat );
    a->khat = ( a->ihat * b->jhat ) - ( a->jhat * b->ihat );
}

double pointToLineDistance( cartesian3 * x_0,
                            cartesian3 * x_1,
                            cartesian3 * x_2 )
{
    vec3 top;
    top.ihat       = x_0->x - x_1->x;
    top.jhat       = x_0->y - x_1->y;
    top.khat       = x_0->z - x_1->z;
    
    vec3 top_alt;
    top_alt.ihat   = x_0->x - x_2->x;
    top_alt.jhat   = x_0->y - x_2->y;
    top_alt.khat   = x_0->z - x_2->z;
    
    vec3 bot;
    bot.ihat       = x_2->x - x_1->x;
    bot.jhat       = x_2->y - x_1->y;
    bot.khat       = x_2->z - x_1->z;
    
    vec3CrossProduct( &top, &top_alt );
    
    return ( lengthOfVec3( &top ) / lengthOfVec3( &bot ) );
}

void cma( double new_val, double *avg, uint8_t num )
{
    *avg += (new_val - *avg)/(num + 1);
}

/*! Check for motion event - Linear, Circular, etc... */
void motionEvent( cartesian3 new_coord,
                  cartesian3 *sample_buffer,
                  uint8_t sample_buffer_size,
                  uint8_t sample_buffer_index,
                  enum motion_pattern *pattern,
                  double *pattern_variance )
{
    cartesian3 M;
    cartesian3 H;
    
    switch( ( uint8_t ) *pattern )
    {
        case NONE:
        { break; }
        case LINE:
        {
            double reg_v = linearRegression3d( sample_buffer, sample_buffer_size, &M, &H );
            double dist = pointToLineDistance( &new_coord, &M, &H );
            cma( dist, pattern_variance, sample_buffer_size );
            break;
        }
        case CIRCLE:
        { break; }
        default:
        { break; }
    }
    if( *pattern_variance > MAX_PATTERN_VARIANCE)
    {
        *pattern = NONE;
    }
}


#endif /* pd_h */
