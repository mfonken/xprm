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
    coord final_c;
    
    norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
    norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
    double theta    = atan2( norm_c.y, norm_c.x );
    double r_n      = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) ) ;
    double r        = ( sqrt( - ( RAD_NORM_F * RAD_NORM_F ) * ( r_n * r_n ) + ( 2 * RAD_NORM_F) * r_n + 1 ) + 2 * r_n - 1 ) / 2;
    deproj_c.x      = r * cos( theta );
    deproj_c.y      = r * sin( theta );
    final_c.x       = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
    final_c.y       = ( ( deproj_c.y + 1 ) * image_height ) / 2;
    img_c->x        = final_c.x;
    img_c->y        = final_c.y;

}

void fisheye(coord *img_c, double image_width, double image_height )
{
    double w = image_width;
    double h = image_height;
    double x = img_c->x;
    double y = img_c->y;
    double ny = ((2*y)/h)-1;
    // pre calculate ny*ny
    double ny2 = ny*ny;
    // for each column
    // normalize x coordinate to -1 ... 1
    double nx = ((2*x)/w)-1;
    // pre calculate nx*nx
    double nx2 = nx*nx;
    // calculate distance from center (0,0)
    // this will include circle or ellipse shape portion
    // of the image, depending on image dimensions
    // you can experiment with images with different dimensions
    double r = sqrt(nx2+ny2);
    // discard pixels outside from circle!
        double nr = sqrt(1.0-r*r);
        // new distance is between 0 ... 1
        nr = (r + (1.0-nr)) / 2.0;
        // discard radius greater than 1.0
            // calculate the angle for polar coordinates
            double theta = atan2(ny,nx);
            // calculate new x position with new distance in same angle
            double nxn = nr*cos(theta);
            // calculate new y position with new distance in same angle
            double nyn = nr*sin(theta);
            // map from -1 ... 1 to image coordinates
            int x2 = (int)(((nxn+1)*w)/2.0);
            // map from -1 ... 1 to image coordinates
            int y2 = (int)(((nyn+1)*h)/2.0);
            // find (x2,y2) position from source pixels
            img_c->x = x2;
            img_c->y = y2;
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
