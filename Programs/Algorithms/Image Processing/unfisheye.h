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

#define SQRT_32     5.65685424949
#define RAD_NORM_F  2

typedef struct _coord
{
    double x;
    double y;
} coord;

void inverseFisheye(coord *img_c, double image_width, double image_height )
{
    coord norm_c;
    coord deproj_c;
    
    norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
    norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
    
    double theta    = atan2( norm_c.y, norm_c.x );
    double r_n      = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) ) ;
    double r        = ( sqrt( ( - ( RAD_NORM_F * RAD_NORM_F ) * ( r_n * r_n ) ) + ( 2 * RAD_NORM_F) * r_n + 1 ) + 2 * r_n - 1 ) / 2;
    deproj_c.x      = r * cos( theta );
    deproj_c.y      = r * sin( theta );
    
    img_c->x        = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
    img_c->y        = ( ( deproj_c.y + 1 ) * image_height ) / 2;

}

void fisheye(coord *img_c, double image_width, double image_height )
{
    coord norm_c;
    coord deproj_c;
    
    norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
    norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
    
    double r        = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) ) ;
    double r_n      = ( r + (1 - sqrt( 1.0 - r * r ) ) ) / RAD_NORM_F;
    double theta    = atan2( norm_c.y, norm_c.x );
    deproj_c.x      = r_n * cos( theta );
    deproj_c.y      = r_n * sin( theta );
    
    img_c->x        = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
    img_c->y        = ( ( deproj_c.y + 1 ) * image_height ) / 2;
}

//norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
//norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
//double theta    = atan2( norm_c.y, norm_c.x );
//double r_n      = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) );
//double r        = ( sqrt( - ( RAD_NORM_F * RAD_NORM_F ) * ( r_n * r_n ) + ( 2 * RAD_NORM_F) * r_n + 1 ) + 2 * r_n - 1 ) / 2;
//deproj_c.x      = r * cos( theta );
//deproj_c.y      = r * sin( theta );
//final_c.x       = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
//final_c.y       = ( ( deproj_c.y + 1 ) * image_height ) / 2;
//img_c->x        = final_c.x;
//img_c->y        = final_c.y;

//coord norm_c;
//coord deproj_c;
//coord final_c;
//
//norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
//norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
//double d_std    = 2 * ( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) );
//double d_inv    =  ( d_std == 0 ) ? 0.00001 : ( 1 / d_std );
//double d_srt    = sqrt( d_std );
//double deproj   = sqrt( d_inv + sqrt( d_inv - ( 2 * d_std ) + ( ( SQRT_32 * d_srt ) - 4 ) ) );
//deproj_c.x      = norm_c.x * deproj;
//deproj_c.y      = norm_c.y * deproj;
//final_c.x       = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
//final_c.y       = ( ( deproj_c.y + 1 ) * image_height ) / 2;
//img_c->x        = final_c.x;
//img_c->y        = final_c.y;

#endif /* unfisheye_h */
