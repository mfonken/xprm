//
//  unfisheye.h
//  
//
//  Created by Matthew Fonken on 10/30/16.
//
//

#ifndef unfisheye_h
#define unfisheye_h

#include <stdint.h>
#include <math.h>
#include "sin_cos_atan.h"
#include "cordic.h"

#define RAD_NORM_F      2
#define RAD_NORM_F_2    RAD_NORM_F * RAD_NORM_F
#define PRECISION       10

coord norm_c;
coord deproj_c;
double s,c;

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

#endif /* unfisheye_h */
