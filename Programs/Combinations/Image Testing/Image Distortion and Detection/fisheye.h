/*! \file fisheye.h
 \brief Fisheye and Inverse Algorithm
 
Created by Matthew Fonken on 10/30/16.
 */


#ifndef fisheye_h
#define fisheye_h

#include <stdint.h>
#include <math.h>
#include "types.h"
#include "sin_cos_atan.h"
#include "cordic.h"

#define RAD_NORM_F      2   /**< Radius normalizing factor */
#define RAD_NORM_F_2    RAD_NORM_F * RAD_NORM_F /**< \f$RAD_{NORM_F}^2\f$ */
#define PRECISION       10  /**< Number of steps for cordic */

coord norm_c;
coord deproj_c;
double s,c;

/*! Inverse Fisheye\r\n
 \f{eqnarray*}{

    &x_{normalized} = \frac{2x_{img}}{w} - 1 \\
    &y_{normalized} = \frac{2y_{img}}{h} - 1 \\
    &r_{normalized} = \sqrt{x_{normalized}^2 + y_{normalized}^2} \\
    &r = \frac{\sqrt{-R_{nf}^2r_{normalized}^2 + 2R_{nf}r_{normalized}+1} + 2r_{normalized} - 1}{2} \\
    &s = \frac{z}{\sqrt{z^2+1}} = \sin(\tan^{-1}(z)) \\
    &c = \frac{1}{\sqrt{z^2+1}} = \cos(\tan^{-1}(z)) \\
    &x_{deprojected} = rs \\
    &y_{deprojected} = rc \\
    &x_{img} = \frac{w(x_{deprojected} + 1)}{2} \\
    &y_{img} = \frac{h(y_{deprojected} + 1)}{2}
 \f}
 */
void inverseFisheye(coord *img_c, double image_width, double image_height )
{
    norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
    norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
    
    double r_n      = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) );
    double r        = ( sqrt( ( - ( RAD_NORM_F_2 ) * ( r_n * r_n ) ) + ( 2 * RAD_NORM_F) * r_n + 1 ) + 2 * r_n - 1 ) / 2;
    
    s_c_a( &norm_c, &s, &c );
    deproj_c.x      = r * c;
    deproj_c.y      = r * s;
    
    img_c->x        = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
    img_c->y        = ( ( deproj_c.y + 1 ) * image_height ) / 2;
}

/*! Fisheye\r\n
 \f{eqnarray*}{
 
 &x_{normalized} = \frac{2x_{img}}{w} - 1 \\
 &y_{normalized} = \frac{2y_{img}}{h} - 1 \\
 &r = \sqrt{x_{normalized}^2 + y_{normalized}^2} \\
 &r_{normalized} = \frac{r + (1 - \sqrt{1 - r^2})}{R_{nf}} \\
 &s = \frac{z}{\sqrt{z^2+1}} = \sin(\tan^{-1}(z)) \\
 &c = \frac{1}{\sqrt{z^2+1}} = \cos(\tan^{-1}(z)) \\
 &x_{deprojected} = rs \\
 &y_{deprojected} = rc \\
 &x_{img} = \frac{w(x_{deprojected} + 1)}{2} \\
 &y_{img} = \frac{h(y_{deprojected} + 1)}{2}
 \f}
 */
void fisheye(coord *img_c, double image_width, double image_height )
{
    norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
    norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
    
    double r        = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) );
    double r_n      = ( r + (1 - sqrt( 1.0 - r * r ) ) ) / RAD_NORM_F;

    s_c_a( &norm_c, &s, &c );
    deproj_c.x      = r_n * c;
    deproj_c.y      = r_n * s;
    
    img_c->x        = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
    img_c->y        = ( ( deproj_c.y + 1 ) * image_height ) / 2;
}

void inverseFisheyeCordic(coord *img_c, double image_width, double image_height )
{
    norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
    norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
    
    double r_n      = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) );
    double r        = ( sqrt( ( - ( RAD_NORM_F_2 ) * ( r_n * r_n ) ) + ( 2 * RAD_NORM_F) * r_n + 1 ) + 2 * r_n - 1 ) / 2;
    
    double theta    = atan2( norm_c.y, norm_c.x );
    cordic( theta, &s, &c, 2 );
    deproj_c.x      = r * c;
    deproj_c.y      = r * s;
    
    img_c->x        = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
    img_c->y        = ( ( deproj_c.y + 1 ) * image_height ) / 2;
}

void fisheyeCordic(coord *img_c, double image_width, double image_height )
{
    norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
    norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
    
    double r        = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) );
    double r_n      = ( r + (1 - sqrt( 1.0 - r * r ) ) ) / RAD_NORM_F;
    
    double theta    = atan2( norm_c.y, norm_c.x );
    cordic( theta, &s, &c, 2 );
    deproj_c.x      = r_n * c;
    deproj_c.y      = r_n * s;
    
    img_c->x        = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
    img_c->y        = ( ( deproj_c.y + 1 ) * image_height ) / 2;
}

void inverseFisheyeStd(coord *img_c, double image_width, double image_height )
{
    norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
    norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
    
    double r_n      = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) );
    double r        = ( sqrt( ( - ( RAD_NORM_F_2 ) * ( r_n * r_n ) ) + ( 2 * RAD_NORM_F) * r_n + 1 ) + 2 * r_n - 1 ) / 2;
    
    double theta    = atan2( norm_c.y, norm_c.x );
    deproj_c.x      = r * cos( theta );
    deproj_c.y      = r * sin( theta );
    
    img_c->x        = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
    img_c->y        = ( ( deproj_c.y + 1 ) * image_height ) / 2;
}

void fisheyeStd(coord *img_c, double image_width, double image_height )
{
    norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
    norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
    
    double r        = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) );
    double r_n      = ( r + (1 - sqrt( 1.0 - r * r ) ) ) / RAD_NORM_F;
    
    double theta    = atan2( norm_c.y, norm_c.x );
    deproj_c.x      = r_n * cos( theta );
    deproj_c.y      = r_n * sin( theta );
    
    img_c->x        = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
    img_c->y        = ( ( deproj_c.y + 1 ) * image_height ) / 2;
}

#endif /* fisheye_h */
