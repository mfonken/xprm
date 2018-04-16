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

#define RAD_NORM_F      1//0.85
#define RAD_NORM_F_2 ( RAD_NORM_F * RAD_NORM_F )

typedef struct { double x, y;} point2_t;

#define INR(X,A,B) X>=A&&X<B

using namespace cv;
using namespace std;

double strength = 0.1, zoom = 0.5;
int strength_slider = 20, zoom_slider = 11, max_strength = 100, max_zoom = 20;

double strength2 = 0.1, zoom2 = 0.5;
int strength_slider2 = 20, zoom_slider2 = 11;

double strength3 = 0.1, zoom3 = 0.5;
int strength_slider3 = 20, zoom_slider3 = 11;

#define LOV_constant 0.414213562
inline double qdist(double ax, double ay) { return (ax > ay )? ax + ay*LOV_constant:ay + ax*LOV_constant; }
inline double qatanr_r( double x ) { return 1.0301 - x*(0.0663*x - 0.1784); }


void fisheye(Mat I, Mat O)
{
    createTrackbar( "Strength", "Out2", &strength_slider2, max_strength );
    createTrackbar( "Zoom", "Out2", &zoom_slider2, max_zoom );
    
    strength2 = (double) strength_slider2 / 10;
    zoom2 = (double) zoom_slider2 / 10;
    
    double w = I.cols, h = I.rows;
    double wh = w/2, hh = h/2;
    Vec3b c = {0,0,0};
    int x, y = 0, x_, y_, xn, yn;
    double theta, r;
  
    double correctionRadius = sqrt((wh*wh)+(hh*hh)) / strength2;
    for(yn = -h/2, y = 0; y < h; yn++, y++ )
    {
        for(xn = -w/2, x = 0; x < w; xn++, x++ )
        {
            r = sqrt((xn*xn)+(yn*yn)) / correctionRadius;
//            if(!r) theta = 1;
            theta = atan(r)/r;
            x_ = wh + theta * xn * zoom2;
            y_ = hh + theta * yn * zoom2;
            if(INR(x_, 0,w) && INR(y_, 0,h))
                O.at<Vec3b>(y_,x_) = I.at<Vec3b>(y,x);
        }
    }
    putText(O, "Strength: " + to_string(strength2), cvPoint(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
    putText(O, "Zoom: " + to_string(zoom2), cvPoint(30,48), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
}

void invfisheye(Mat I, Mat O)
{
    createTrackbar( "Strength", "Out3", &strength_slider3, max_strength );
    createTrackbar( "Zoom", "Out3", &zoom_slider3, max_zoom );
    
    strength3 = (double) strength_slider3 / 10;
    zoom3 = (double) zoom_slider3 / 10;
    
    double w = I.cols, h = I.rows;
    double wh = w/2, hh = h/2;
    Vec3b c = {0,0,0};
    int x, y = 0, x_, y_, xn, yn;
    double theta, r;
    
    double correctionRadius = sqrt((wh*wh)+(hh*hh)) / strength3;
    for(yn = -h/2, y = 0; y < h; yn++, y++ )
    {
        for(xn = -w/2, x = 0; x < w; xn++, x++ )
        {
            r = sqrt((xn*xn)+(yn*yn)) / correctionRadius;
            if(!r) theta = 1;
            else theta = r / atan(r);
            x_ = wh + xn * theta * zoom3;
            y_ = hh + yn * theta * zoom3;
            if(INR(x_, 0,w) && INR(y_, 0,h))
                O.at<Vec3b>(y_,x_) = I.at<Vec3b>(y,x);
        }
    }
    putText(O, "Strength: " + to_string(strength3), cvPoint(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
    putText(O, "Zoom: " + to_string(zoom3), cvPoint(30,48), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
}

#endif /* unfisheye_h */
