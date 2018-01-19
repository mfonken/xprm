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
#include <highgui.h>

//#define PERFORMANCE_TEST
//#define QUICK
//#define ALT_QUICK

//#define SQRT_32     5.65685424949
//#define RAD_NORM_F  0.85
//#define RAD_NORM_F_2 ( RAD_NORM_F * RAD_NORM_F )
//#define DIV_F       1

typedef struct { double x, y;} point2_t;

#define INR(X,A,B) X>=A&&X<B

using namespace cv;
using namespace std;

double strength = 0.1, zoom = 0.5;
int strength_slider = 20, zoom_slider = 11, max_strength = 100, max_zoom = 20;

#define LOV_constant 0.414213562
inline double qdist(double ax, double ay) { return (ax > ay )? ax + ay*LOV_constant:ay + ax*LOV_constant; }
inline double qatanr_r( double x ) { return 1.0301 - x*(0.0663*x - 0.1784); }

void quickUnfisheye(Mat I, Mat O)
{
#ifndef PERFORMANCE_TEST
    createTrackbar( "Strength", "Out", &strength_slider, max_strength );
    createTrackbar( "Zoom", "Out", &zoom_slider, max_zoom );
    
    strength = (double) strength_slider / 10;
    zoom = (double) zoom_slider / 10;
#endif
    
    double w = I.rows, h = I.rows;
    double wh = w/2, hh = h/2;
    int x, y = 0, x_, y_, xn, yn;
    double theta_zoom, r;
    
    double correctionRadius_ = (1 / qdist(fabs(wh), fabs(hh))) * strength;
    for(yn = -hh, y = 0; y < h; yn++, y++ )
    {
        for(xn = -wh, x = 0; x < w; xn++, x++ )
        {
            r = qdist(fabs(xn), fabs(yn)) * correctionRadius_;
            theta_zoom = qatanr_r(r) * zoom;
            x_ = wh + theta_zoom * xn;
            y_ = hh + theta_zoom * yn;
            O.at<Vec3b>(y,x) = I.at<Vec3b>(y_,x_);
        }
    }
    
#ifndef PERFORMANCE_TEST
    putText(O, "Strength: " + to_string(strength), cvPoint(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
    putText(O, "Zoom: " + to_string(zoom), cvPoint(30,48), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
#endif
}

void unfisheye(Mat I, Mat O)
{
    createTrackbar( "Strength", "Out", &strength_slider, max_strength );
    createTrackbar( "Zoom", "Out", &zoom_slider, max_zoom );
    
    strength = (double) strength_slider / 10;
    zoom = (double) zoom_slider / 10;
    
    double w = I.cols, h = I.rows;
    double wh = w/2, hh = h/2;
    Vec3b c = {0,0,0};
    int x, y = 0, x_, y_, xn, yn;
    double theta, r;

#ifdef QUICK
    double correctionRadius_ = 1 / sqrt((wh*wh)+(hh*hh)) * strength;
    for(yn = -h/2, y = 0; y < h; yn++, y++ )
    {
        for(xn = -w/2, x = 0; x < w; xn++, x++ )
        {
            r = sqrt((xn*xn)+(yn*yn)) * correctionRadius_;
            theta = qatanr_r(r) * zoom;
            x_ = wh + theta * xn;// * zoom;
            y_ = hh + theta * yn;// * zoom;
            O.at<Vec3b>(y,x) = I.at<Vec3b>(y_,x_);
        }
    }
#endif
#ifndef QUICK
#ifdef ALT_QUICK
    double correctionRadius_ = (1 / qdist(fabs(wh), fabs(hh))) * strength, thetaz;
    for(yn = -h/2, y = 0; y < h; yn++, y++ )
    {
        for(xn = -w/2, x = 0; x < h; xn++, x++ )
        {
            r = qdist(fabs(xn), fabs(yn)) * correctionRadius_;
            thetaz = qatanr_r(r) * zoom;
            x_ = wh + thetaz * xn;
            y_ = hh + thetaz * yn;
            O.at<Vec3b>(y,x) = I.at<Vec3b>(y_,x_);
        }
    }
#else
    double correctionRadius = sqrt((wh*wh)+(hh*hh)) / strength;
    for(yn = -h/2, y = 0; y < h; yn++, y++ )
    {
        for(xn = -w/2, x = 0; x < w; xn++, x++ )
        {
            r = sqrt((xn*xn)+(yn*yn)) / correctionRadius;
            if(!r) theta = 1;
            else theta = atan(r)/r;
            x_ = wh + theta * xn * zoom;
            y_ = hh + theta * yn * zoom;
            if(INR(x_, 0,w) && INR(y_, 0,h))
                O.at<Vec3b>(y,x) = I.at<Vec3b>(y_,x_);
        }
    }
#endif
#endif
    putText(O, "Strength: " + to_string(strength), cvPoint(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
    putText(O, "Zoom: " + to_string(zoom), cvPoint(30,48), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);

}

//void fisheye(Mat I, Mat O)
//{
//    double w = I.cols, h = I.rows;
//    int x_, y_;
//    for( int y = 0; y < h; y++ )
//    {
//        for( int x = 0; x < w; x++ )
//        {
//            Vec3b c = I.at<Vec3b>(y,x);
//            norm_c.x        = ( 2 * x ) / w;
//            norm_c.y        = ( 2 * y ) / h;
//
//            double r        = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) ) ;
//            double r_n      = ( r + (1 - sqrt( 1.0 - r * r ) ) ) / RAD_NORM_F;
//            double theta    = atan2( norm_c.y, norm_c.x );
//            deproj_c.x      = r_n * cos( theta );
//            deproj_c.y      = r_n * sin( theta );
//
//            x_ = ( deproj_c.x * w ) / 2;
//            y_ = ( deproj_c.y * h ) / 2;
//
//            if(INR(x_,0,w) && INR(y_,0,h))
//            {
//                O.at<Vec3b>(y_,x_) = c;
//            }
//        }
//    }
//}
//void inverseFisheye(Mat I, Mat O)
//{
//    double w = I.cols, h = I.rows;
//    Vec3b c = {0,0,0};
//    int x, y = 0, x_, y_;
//    double xn, yn = -1, xni = 2/w, yni = 2/h;
//    double theta, r_n_sq, r_n, r;
//    for(; y < h; yn+=yni, y++ )
//    {
//        x = 0;
//        for( xn = -1; x < w; xn+=xni, x++ )
//        {
//            c = I.at<Vec3b>(y,x);
//
//            theta = atan2( yn, xn );
//            r_n_sq = ( xn * xn ) + ( yn * yn );
//            r_n = sqrt( r_n_sq );
//            r = ( sqrt( ( - RAD_NORM_F_2 * r_n_sq ) + ( 2 * RAD_NORM_F) * r_n + 1 ) + 2 * r_n - 1 ) / 2;
//            if(INR(r,0.0,1.0))
//            {
//                x_ = ( (r * cos( theta ) + 1) * w ) / 2;
//                y_ = ( (r * sin( theta ) + 1) * h ) / 2;
//                O.at<Vec3b>(y_,x_) = c;
//            }
//        }
//    }
//}
#endif /* unfisheye_h */
