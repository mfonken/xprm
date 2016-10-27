//
//  pd.h
//  
//
//  Created by Matthew Fonken on 10/15/16.
//
//

#ifndef pd_h
#define pd_h

#include "tm.h"

#ifndef PI
#define PI 3.14159
#endif

#define MAX_PATTERN_VARIANCE    1.0

enum motion_pattern
{
    NONE = 0,
    LINE,
    CIRCLE,
};

struct cartesian3
{
    double x;
    double y;
    double z;
};

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

/* 3D Linear Regression - See https://www.scribd.com/doc/31477970/Regressions-et-trajectoires-3D#download */
double linearRegression3d( struct cartesian3 * samples,
                           uint8_t num_of_samples,
                           struct cartesian3 * M,
                           struct cartesian3 * H )
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
    
    double S_x_x    = 0;
    double S_y_y    = 0;
    double S_z_z    = 0;
    double S_x_y    = 0;
    double S_x_z    = 0;
    double S_y_z    = 0;
    
    S_x_x = - ( X_m * X_m ) + X_m_X_m;
    S_y_y = - ( Y_m * Y_m ) + Y_m_Y_m;
    S_z_z = - ( Z_m * Z_m ) + Z_m_Z_m;
    S_x_x = - ( X_m * Y_m ) + X_m_Y_m;
    S_x_x = - ( X_m * Z_m ) + X_m_Z_m;
    S_x_x = - ( Y_m * Z_m ) + Y_m_Z_m;
    
    double t = 0.5 * atan2( 2 * S_x_y, S_x_x - S_y_y );
    double cos_a = cos( t );
    double sin_a = sin( t );
    double cos_a_cos_a = cos_a * cos_a;
    double sin_a_sin_a = sin_a * sin_a;
    double K_1_1 = ( S_y_y + S_z_z ) * cos_a_cos_a + ( S_x_x + S_z_z ) * sin_a_sin_a - 2 * S_x_y * cos_a * sin_a;
    double K_2_2 = ( S_y_y + S_z_z ) * sin_a_sin_a + ( S_x_x + S_z_z ) * cos_a_cos_a + 2 * S_x_y * cos_a * sin_a;
    double K_1_2 = - S_x_y * (cos_a_cos_a - sin_a_sin_a ) + ( S_x_x - S_y_y ) * cos_a * sin_a;
    double K_1_0 =   S_x_z * cos_a + S_y_z * sin_a;
    double K_0_1 = - S_x_z * sin_a + S_y_z * cos_a;
    double K_0_0 =   S_x_x + S_y_y;
    
    double c_0   = ( ( K_0_1 * K_0_1 ) * K_1_1 ) + ( ( K_1_0 * K_1_0 ) * K_2_2 ) - K_0_0 * K_1_1 * K_2_2;
    double c_1   = ( K_0_0 * K_1_1 ) + ( K_0_0 * K_2_2 ) + ( K_1_1 * K_2_2 ) - ( K_0_1 * K_0_1 ) - ( K_1_0 * K_1_0 );
    double c_2   = - K_0_0 - K_1_1 - K_2_2;
    
    double p     = c_1 - ( 1 / 3 ) * ( c_2 * c_2 );
    double q     = ( 2 / 27 ) * ( c_2 * c_2 * c_2 ) - ( 1 / 3 ) * c_1 * c_2 + c_0;
    double R     = ( 1 / 4 ) * ( q * q ) + ( 1 / 27 ) * ( p * p * p );
    double d_m_sq = 0;
    
    if( R > 0 )
    {
        double R_sqrt = sqrt( R );
        d_m_sq = - ( 1 / 3 ) * c_2 + cbrt( - 0.5 * q + R_sqrt ) + cbrt( - 0.5 * q - R_sqrt );
    }
    else
    {
        double r = sqrt( - ( 1 / 27 ) * ( p * p * p ) );
        
        double f_c = ( 1 / 3 ) * acos( - q / ( 2 * r ) );
        double c_2_c = - ( 1 / 3 ) * c_2;
        double r_c = 2 * cbrt( r );
        
        double d_m_sq_0 = c_2_c + r_c * cos( f_c );
        double d_m_sq_1 = c_2_c + r_c * cos( f_c + 2 * PI );
        double d_m_sq_2 = c_2_c + r_c * cos( f_c + 4 * PI );
        
        d_m_sq = minTriple( d_m_sq_0, d_m_sq_1, d_m_sq_2 );
    }
    
    /* Regression Line Calculation */
    double K_c_0    = ( - K_1_0 / ( K_1_1 - d_m_sq ) );
    double K_c_1    = ( - K_0_1 / ( K_2_2 - d_m_sq ) );
    double a        = K_c_0 * cos_a + K_c_1 * sin_a;
    double b        = K_c_0 * sin_a + K_c_1 * cos_a;
    double H_c      = ( 1 / ( 1 + ( a * a ) + ( b * b ) ) );
    double u        = H_c * ( ( 1 + ( b * b ) ) * X_m - ( ( a * b ) * Y_m ) + ( a * Z_m ) );
    double v        = H_c * ( ( - ( a * b ) * X_m ) + ( ( 1 + ( a * a ) ) * Y_m ) + ( b * Z_m ) );
    double w        = H_c * ( ( a * X_m ) - ( b * Y_m ) + ( ( ( a * a ) + ( b * b ) ) * Z_m ) );
    
    M->x = X_m;
    M->y = Y_m;
    M->z = Z_m;
    
    H->x = u;
    H->y = v;
    H->z = w;
    
    return d_m_sq;
}

/* Cross Product of Two 3D Vectors - See https://en.wikipedia.org/wiki/Cross_product */
void vec3CrossProduct( struct vec3 * a,
                       struct vec3 * b )
{
    a->ihat = ( a->jhat * b->khat ) - ( a->khat * b->jhat );
    a->jhat = ( a->khat * b->ihat ) - ( a->ihat * b->khat );
    a->khat = ( a->ihat * b->jhat ) - ( a->jhat * b->ihat );
}

/* Point to Line Distance in 3D Space - See http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html */
double pointToLineDistance( struct cartesian3 * x_0,
                            struct cartesian3 * x_1,
                            struct cartesian3 * x_2 )
{
    struct vec3 top;
    top.ihat       = x_0->x - x_1->x;
    top.jhat       = x_0->y - x_1->y;
    top.khat       = x_0->z - x_1->z;
    
    struct vec3 top_alt;
    top_alt.ihat   = x_0->x - x_2->x;
    top_alt.jhat   = x_0->y - x_2->y;
    top_alt.khat   = x_0->z - x_2->z;
    
    struct vec3 bot;
    bot.ihat       = x_2->x - x_1->x;
    bot.jhat       = x_2->y - x_1->y;
    bot.khat       = x_2->z - x_1->z;
    
    vec3CrossProduct( &top, &top_alt );
    
    return ( lengthOfVec3( &top ) / lengthOfVec3( &bot ) );
}

/* Cumulative Moving Average - See https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average */
void cma( double new_val, double *avg, uint8_t num )
{
    *avg += (new_val - *avg)/(num + 1);
}

void motionEvent( struct cartesian3 new_coord,
                  struct cartesian3 *sample_buffer,
                  uint8_t sample_buffer_size,
                  uint8_t sample_buffer_index,
                  enum motion_pattern *pattern,
                  double *pattern_variance )
{
    struct cartesian3 M;
    struct cartesian3 H;
    
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
