//
//  fisheye_math.h
//  fisheyetest
//
//  Created by Matthew Fonken on 1/2/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef fisheye_math_h
#define fisheye_math_h

#include <math.h>

typedef struct { int x, y;} point2_t;
void pp2(const char * s, point2_t * a) { printf("%s(%d, %d)\n", s, a->x, a->y); }

double strength = 0.1, zoom = 0.5;
int strength_slider = 20, zoom_slider = 11, max_strength = 100, max_zoom = 20;

void ps(void){ printf("Setup is strength %.2f and zoom %.2f\n", (double) strength_slider / 10, (double) zoom_slider / 10); }

#define INR(X,A,B) X>=A&&X<B

void unfisheye(point2_t * I, point2_t * O, double w, double h)
{
    strength = (double) strength_slider / 10;
    zoom = (double) zoom_slider / 10;
    
    double wh = w/2, hh = h/2;
    int x_, y_, xn = I->x-wh, yn = I->y-hh;
    double theta, r;
    
    double correctionRadius = sqrt((wh*wh)+(hh*hh)) / strength;
    r = sqrt((xn*xn)+(yn*yn)) / correctionRadius;
    if(!r) theta = 1;
    else theta = atan(r)/r;
    x_ = wh + theta * xn * zoom;
    y_ = hh + theta * yn * zoom;
    if(INR(x_, 0,w) && INR(y_, 0,h))
    {
        O->x = x_;
        O->y = y_;
    }
}

void fisheye(point2_t * I, point2_t * O, double w, double h)
{
    strength = (double) strength_slider / 10;
    zoom = (double) zoom_slider / 10;
    
    double wh = w/2, hh = h/2;
    int x_, y_, xn = I->x-wh, yn = I->y-hh;
    double theta, r;
    
    double correctionRadius = sqrt((wh*wh)+(hh*hh)) / strength;
    r = sqrt((xn*xn)+(yn*yn)) / correctionRadius;
    if(!r) theta = 1;
    else theta = (atan(r)/r) * zoom;
    x_ = wh + xn / theta;
    y_ = hh + yn / theta;
    if(INR(x_, 0,w) && INR(y_, 0,h))
    {
        O->x = x_;
        O->y = y_;
    }
}

#endif /* fisheye_math_h */
