/*! \file pd.h
    \brief Polynomial Degression
 
    Created by Matthew Fonken on 10/15/16.
*/

#ifndef pd_h
#define pd_h

#include "tm.h"

#ifndef PI
#define PI 3.14159
#endif

#define MAX_PATTERN_VARIANCE    1.0 /**< Maximum pattern variance */

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

/*! 3D Linear Regression\r\n
    - See https://www.scribd.com/doc/31477970/Regressions-et-trajectoires-3D
 \f{eqnarray*}{
    &X_m = \frac{1}{n}\sum_{k=1}^n X_k, Y_m = \frac{1}{n}\sum_{k=1}^n Y_k, Z_m = \frac{1}{n}\sum_{k=1}^n Z_k \\\\
    &\begin{cases}
        S_{xx} = -X_m^2 + \frac{1}{n}\sum_{k=1}^n X_k^2, S_{xy} = -X_mY_m + \frac{1}{n}\sum_{k=1}^n X_kY_k \\
        S_{yy} = -Y_m^2 + \frac{1}{n}\sum_{k=1}^n Y_k^2, S_{xz} = -X_mZ_m + \frac{1}{n}\sum_{k=1}^n X_kZ_k \\
        S_{zz} = -Z_m^2 + \frac{1}{n}\sum_{k=1}^n Z_k^2, S_{yz} = -Y_mZ_m + \frac{1}{n}\sum_{k=1}^n Y_kZ_k \\
    \end{cases} \\
    &\theta = \frac{1}{2}\tan^{-1}(\frac{2S_{xy}}{S_{xx} - S_{yy}}) \\
    &\begin{cases}
        K_{11} = (S_{yy} + S_{zz})\cos^2(\theta) + (S_{xx} + S_{zz})\sin^2(\theta) - 2S_{xy}\cos(\theta)\sin(\theta) \\
        K_{22} = (S_{yy} + S_{zz})\sin^2(\theta) + (S_{xx} + S_{zz})\cos^2(\theta) - 2S_{xy}\cos(\theta)\sin(\theta) \\
        K_{12} = -S_{xy}(\cos^2(\theta) - \sin^2(\theta))+ (S_{xx} - S_{yy})\cos(\theta)\sin(\theta) \\
        K_{10} = S_{xz}\cos(\theta) + S_{yz}\sin(\theta) \\
        K_{01} = -S_{xz}\sin(\theta) + S_{yz}\cos(\theta) \\
        K_{00} = S_{xx} + S_{yy} \\
    \end{cases} \\
    &\begin{cases}
        c_2 = -K_{00} - K_{11} - K_{22} \\
        c_1 = K_{00}K_{11} + K_{00}K_{22} + K_{11}K_{22} - K_{01}^2 - K_{10}^2 \\
        c_0 = K_{01}^2K_{11} + K_{10}^2K_{22} - K_{00}K_{11}K_{22} \\
    \end{cases} \\
    &p = c_1 - \frac{1}{3}c_2^2, q = \frac{2}{27}c_2^3 - \frac{1}{3}c_1c_2 + c_0, R = \frac{1}{4}q^2 + \frac{1}{27}p^3 \\
    \mathbf{If\space R > 0:} \\
    &\delta_m^2 = -\frac{1}{3}c_2+\sqrt[3]{-\frac{1}{2}q + \sqrt{R}} + \sqrt[3]{-\frac{1}{2}q - \sqrt{R}} \\
    \mathbf{If\space R < 0:} \\
    &\rho = \sqrt{-\frac{1}{27}p^3}, \phi = \cos^{-1}(-\frac{q}{2rho}) \\
    &\delta_m^2 =
    \begin{cases}
        -\frac{1}{3}c_2 + 2\sqrt[3]{\rho}\cos(\frac{1}{3}\rho) \\
        -\frac{1}{3}c_2 + 2\sqrt[3]{\rho}\cos(\frac{1}{3}\rho + 2\pi) \\
        -\frac{1}{3}c_2 + 2\sqrt[3]{\rho}\cos(\frac{1}{3}\rho + 4\pi) \\
    \end{cases} \\
    \mathbf{\text{Regression Line Calculation:}} \\
    &\begin{cases}
        a = -\frac{K_{10}}{K_{11}-\delta_m^2}\cos(\theta) + -\frac{K_{01}}{K_{22}-\delta_m^2}\sin(\theta) \\
        b = -\frac{K_{10}}{K_{11}-\delta_m^2}\sin(\theta) + -\frac{K_{01}}{K_{22}-\delta_m^2}\cos(\theta) \\
    \end{cases} \\
    &\begin{cases}
        u = \frac{1}{1+a^2+b^2}((1 + b^2)X_m - abY_m + aZ_m) \\
        v = \frac{1}{1+a^2+b^2}(-abX_m + (1 + a^2)Y_m + bZ_m) \\
        w = \frac{1}{1+a^2+b^2}(aX_m + bY_m + (a^2 + b^2)Z_m)
    \end{cases}
 \f}
 */
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
    double S_X_Y_cos_sin = 2 * S_x_y * cos_a * sin_a;
    double K_1_1 = ( S_y_y + S_z_z ) * cos_a_cos_a + ( S_x_x + S_z_z ) * sin_a_sin_a - S_X_Y_cos_sin;
    double K_2_2 = ( S_y_y + S_z_z ) * sin_a_sin_a + ( S_x_x + S_z_z ) * cos_a_cos_a + S_X_Y_cos_sin;
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

/*! Cross Product of Two 3D Vectors\r\n
    - See https://en.wikipedia.org/wiki/Cross_product\r\n
 \f{eqnarray*}{
    &\mathbf{a_{return}} =
    \begin{cases}
        a_{\hat{i}} = a_{\hat{j}}b_{\hat{k}} - a_{\hat{k}}b_{\hat{j}} \\
        a_{\hat{j}} = a_{\hat{k}}b_{\hat{i}} - a_{\hat{i}}b_{\hat{k}} \\
        a_{\hat{k}} = a_{\hat{i}}b_{\hat{j}} - a_{\hat{j}}b_{\hat{i}}
    \end{cases}
 \f}
 */
void vec3CrossProduct( struct vec3 * a,
                       struct vec3 * b )
{
    a->ihat = ( a->jhat * b->khat ) - ( a->khat * b->jhat );
    a->jhat = ( a->khat * b->ihat ) - ( a->ihat * b->khat );
    a->khat = ( a->ihat * b->jhat ) - ( a->jhat * b->ihat );
}

/*! Point to Line Distance in 3D Space\r\n
 - See http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
 \f{eqnarray*}{
    &\mathbf{t_1} =
    \begin{cases}
        t_{1_\hat{i}} = x_0 - x_1 \\
        t_{1_\hat{j}} = y_0 - y_1 \\
        t_{1_\hat{k}} = z_0 - z_1 \\
    \end{cases} \\
    &\mathbf{t_2} =
    \begin{cases}
        t_{2_\hat{i}} = x_0 - x_2 \\
        t_{2_\hat{j}} = y_0 - y_2 \\
        t_{2_\hat{k}} = z_0 - z_2 \\
    \end{cases} \\
    &\mathbf{b} =
    \begin{cases}
        b_{\hat{i}} = x_2 - x_1 \\
        b_{\hat{j}} = y_2 - y_1 \\
        b_{\hat{k}} = z_2 - z_1 \\
    \end{cases} \\
    &d = \frac{||\mathbf{t_1} \times \mathbf{t_2}||}{||\mathbf{b}||}
 \f}
 */
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

/*! Cumulative Moving Average\r\n
    - See https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average 
 */
void cma( double new_val, double *avg, uint8_t num )
{
    *avg += (new_val - *avg)/(num + 1);
}

/*! Check for motion event - Linear, Circular, etc... */
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
