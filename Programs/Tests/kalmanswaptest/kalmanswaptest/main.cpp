//
//  main.cpp
//  kalmanswaptest
//
//  Created by Matthew Fonken on 2/18/18.
//  CopyRIGHT © 2018 Marbl. All RIGHTs reserved.
//

#include <iostream>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "kalman.hpp"
#include "sweeper.hpp"
#include "environment_master.h"

#define STEPS 100
#define SWEEPER_FPS 50
#define FRAME_FPS 50
#define FRAME_DELAY 1000/FRAME_FPS

#define WIDTH 800
#define HEIGHT 100

#define CIRCLE_RADIUS 10
#define CIRCLE_COLOR_A  Scalar(0,100,155)
#define CIRCLE_COLOR_B  Scalar(155,100,0)
#define CIRCLE_SIZE Size(CIRCLE_RADIUS,CIRCLE_RADIUS)

#define LS 1
#define VU 0.1
#define BU 0.01
#define SU 0.01

using namespace cv;

void drawReals(Mat M, int al, int bl)
{
    line(M, Point(al,0), Point(al, HEIGHT), CIRCLE_COLOR_A);
    line(M, Point(bl,0), Point(bl, HEIGHT), CIRCLE_COLOR_B);
}

void drawKalmans( Mat M, KalmanFilter A, KalmanFilter B )
{
    Point p1(A.value, HEIGHT/2), p2(B.value, HEIGHT/2);
    circle(M, p1, CIRCLE_RADIUS, CIRCLE_COLOR_A, -1);
    circle(M, p2, CIRCLE_RADIUS, CIRCLE_COLOR_B, -1);
}

void dualTest(KalmanFilter * K, double v1, double v2)
{
    double x_ = K->value,
    v = K->velocity;//(K->velocity+prev_velocity)/2;

    double x = x_ + v;
    double p1 = fabs(x-v1), p2 = fabs(x-v2);
    
//    printf("x:%.1f | x_:%.1f | v1:%.1f | v2:%.1f | p1:%.1f | p2:%.1f | dt:%.3f | v:%.1f | pv:%.1f | dv:%.3f\n", x, x_, v1, v2, p1, p2, dt, v, prev_velocity, dv);
    
    if( p1 < p2 )
        K->update(v1, v);
    else
        K->update(v2, v);
}

void matchKalmans(KalmanFilter * K1, KalmanFilter * K2, double i1, double i2)
{
    double x_1 = K1->value, /*x_2 = K2->value,*/ v1 = K1->velocity, v2 = K2->velocity;
    double x1 = x_1 + v1;//, x2 = x_2 + v2;
    double p11 = fabs(x1-i1), p12 = fabs(x1-i2);//, p21 = fabs(x2-i1), p22 = fabs(x2-i2);
    
    //    printf("x:%.1f | x_:%.1f | v1:%.1f | v2:%.1f | p1:%.1f | p2:%.1f | dt:%.3f | v:%.1f | pv:%.1f | dv:%.3f\n", x, x_, v1, v2, p1, p2, dt, v, prev_velocity, dv);
    
    if( p11 < p12 )
    {
        K1->update(i1, v1);
        K2->update(i2, v2);
    }
    else
    {
        K1->update(i2, v1);
        K2->update(i1, v2);
    }
}

bool randomSwap( double *a, double *b )
{
    if(rand()%2)
    {
        double t = (*a);
        (*a) = (*b);
        (*b) = t;
        return 1;
    }
    return 0;
}

int main(int argc, const char * argv[]) {
    double Astart = WIDTH/10, Bstart = WIDTH - Astart;
    KalmanFilter A(Astart, LS, VU, BU, SU), B(Bstart, LS, VU, BU, SU);
    Sweeper swA("Sweeper A", STEPS/2, Astart, Bstart);
    Sweeper swB("Sweeper B", STEPS, Bstart, Astart);
    
    Environment env(&swA, SWEEPER_FPS/2);
    env.addTest(&swB, SWEEPER_FPS);
    env.start();
    
    double areal, breal;
    
    bool live = true;
    
    while(1)
    {
        Mat O(HEIGHT, WIDTH, CV_8UC3, Scalar(15,15,15));
    
        areal = swA.value;
        breal = swB.value;
        
        if(live)
        {
            randomSwap(&areal, &breal);
            matchKalmans(&A, &B, areal, breal);
        }
        
        drawKalmans(O, A, B);
        drawReals(O, areal, breal);
        
        imshow("Out Frame", O);
        switch(waitKey(FRAME_DELAY))
        {
            case ' ':
                if(env.status == LIVE)
                {
                    live = false;
                    env.pause();
                }
                else
                {
                    live = true;
                    env.resume();
                }
                break;
            case 02:
                toggle(&swA.dir);
                toggle(&swB.dir);
                swA.trigger();
                swB.trigger();
                toggle(&swA.dir);
                toggle(&swB.dir);
            case 03:
                swA.trigger();
                swB.trigger();
                break;
            default:
                break;
        }
    }
    
    return 0;
}
