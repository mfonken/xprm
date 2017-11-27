//
//  main.cpp
//  tangentsurfacetest
//
//  Created by Matthew Fonken on 11/24/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include <math.h>

#define RAD_TO_DEG (180 / M_PI)
#define DEG_TO_RAD (M_PI / 180)

#define ANGLE   120//63.6396
#define ANGLE_RAD ANGLE * DEG_TO_RAD

#define DIM     500
#define DIV     10

//#define SQ

//#define STEP_ANGLE

using namespace cv;

double proj(double r)
{
    return DIM/15*tan(r);
}

int main(int argc, const char * argv[])
{
    double dim = DIM, div = DIV;
    Mat A(dim, dim, CV_8UC3, Scalar(0,0,0));
    Mat B(dim, dim, CV_8UC3, Scalar(0,0,0));
    Mat C(dim, dim, CV_8UC3, Scalar(0,0,0));
    Mat D(dim, dim, CV_8UC3, Scalar(0,0,0));
    double the,phi,a,r,R;
    int x, y, xt, yt, xf, yf;
    
    for(x = 0; x <= dim; x += dim/div)
    {
        for(y = 0; y < dim; y ++)
        {
            the = ANGLE*DEG_TO_RAD*(x/dim-0.5);
            phi = ANGLE*DEG_TO_RAD*(y/dim-0.5);
            xt = x - dim/2;
            yt = y - dim/2;
//#ifdef SQ
            r = sqrt(the*the+phi*phi);
            a = atan2(yt, xt);
            
            R = proj(r);
            xf = R*cos(a) + dim/2;
            yf = R*sin(a) + dim/2;
            
            if(xf >=0 && xf < dim && yf >= 0 && yf < dim)
                A.at<Vec3b>(yf,xf) = Vec3b(0,0,255);
//#else
            r = acos(cos(the)*cos(phi));
            a = atan2(yt, xt);
            
            R = proj(r);
            xf = R*cos(a) + dim/2;
            yf = R*sin(a) + dim/2;
            
            if(xf >=0 && xf < dim && yf >= 0 && yf < dim)
                A.at<Vec3b>(yf,xf) = Vec3b(0,255,0);
//#endif
            
        }
    }
      
    for(y = 0; y <= dim; y += dim/div)
    {
        for(x = 0; x < dim; x ++)
        {
            the = ANGLE*DEG_TO_RAD*(x/dim-0.5);
            phi = ANGLE*DEG_TO_RAD*(y/dim-0.5);
            xt = x - dim/2;
            yt = y - dim/2;
//#ifdef SQ
            r = sqrt(the*the+phi*phi);
            a = atan2(yt, xt);
            
            R = proj(r);
            xf = R*cos(a) + dim/2;
            yf = R*sin(a) + dim/2;
            
            if(xf >=0 && xf < dim && yf >= 0 && yf < dim)
                A.at<Vec3b>(yf,xf) = Vec3b(0,0,255);
//#else
            r = acos(cos(the)*cos(phi));
            a = atan2(yt, xt);
            
            R = proj(r);
            xf = R*cos(a) + dim/2;
            yf = R*sin(a) + dim/2;
            
            if(xf >=0 && xf < dim && yf >= 0 && yf < dim)
                A.at<Vec3b>(yf,xf) = Vec3b(0,255,0);
//#endif
        }
    }

    imshow("A", A);
    waitKey(100000);
    return 0;
}
