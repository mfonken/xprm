//
//  main.cpp
//  pidtest
//
//  Created by Matthew Fonken on 12/25/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include "rho_pid.h"
#include <unistd.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

#define TARGET  0.004//1000
#define RANGE   0.50//50

#define RSCALE  10000
#define RTARGET ((int)(TARGET*RSCALE))
#define RRANGE  ((int)(RANGE*RSCALE))
#define RAND    ((floating_t)(rand()%(RRANGE*2)+RTARGET-RRANGE)/((floating_t)RSCALE))

#define RPERIOD                 100
#define NUM_PERIODS_ON_FRAME    3
#define FRAME_HORZ_SCALE        3
#define FRAME_VERT_STEPS_OVER   20
#define FRAME_VERT_STEPS_UNDER  10
#define FRAME_VERT_SCALED_STEP_HEIGHT 30
#define FRAME_VERT_ZERO         (FRAME_VERT_STEPS_OVER*FRAME_VERT_SCALED_STEP_HEIGHT)
#define FRAME_VERT_STEP_HEIGHT  PID_SCALE
#define FRAME_VERT_SCALE        (FRAME_VERT_SCALED_STEP_HEIGHT/FRAME_VERT_STEP_HEIGHT)
#define FRAME_WIDTH             (RPERIOD*NUM_PERIODS_ON_FRAME*FRAME_HORZ_SCALE)
#define FRAME_HEIGHT            (FRAME_VERT_SCALED_STEP_HEIGHT*(FRAME_VERT_STEPS_OVER+FRAME_VERT_STEPS_UNDER))

#define LOOP    1000
#define FPS     15
#define DELAY   (1000/FPS)

void drawGrid(Mat &frame)
{
    rectangle(frame, Point(0,0), Point(frame.cols,frame.rows), Scalar(245,245,245));
    line(frame, Point(0,FRAME_VERT_ZERO), Point(FRAME_WIDTH,FRAME_VERT_ZERO), Scalar(100,100,100));
    for(int i = 1; i <= NUM_PERIODS_ON_FRAME; i++)
        line(frame, Point(RPERIOD*FRAME_HORZ_SCALE*i,0), Point(RPERIOD*FRAME_HORZ_SCALE*i,FRAME_HEIGHT), Scalar(100,100,100));
    
    int max_div = FRAME_VERT_STEPS_OVER*FRAME_VERT_SCALED_STEP_HEIGHT;
    for(int i = 0; i < (FRAME_VERT_STEPS_OVER+FRAME_VERT_STEPS_UNDER); i++)
    {
        int y = FRAME_VERT_SCALED_STEP_HEIGHT*i;
        line(frame, Point(0,y), Point(FRAME_WIDTH,y), Scalar(150,150,150));
        
        std::string div_name = std::to_string(((floating_t)(max_div - y) * FRAME_VERT_STEP_HEIGHT)/FRAME_VERT_SCALED_STEP_HEIGHT);
        putText(frame, div_name, Point(10,y), FONT_HERSHEY_PLAIN, 1, Scalar(200,0,0));
    }
}

int main(int argc, const char * argv[])
{
    rho_pid_t PID;    
    RhoPID.Init(&PID, DEFAULT_PID_GAIN);
    PID.Value = TARGET;
    
    floating_t actual = 0;//TARGET - RANGE;
    floating_t target = TARGET;
    
    Mat frame(Size(FRAME_WIDTH, FRAME_HEIGHT), CV_8UC3, Scalar(245,245,245));
    drawGrid(frame);
    int x = 0;
    Point a(FRAME_VERT_ZERO,0),b(0,FRAME_VERT_ZERO);
    for(int i = 0; i < LOOP; i++)
    {
//        char c = waitKey(DELAY);
//        if(c == ' ')
//        {
//            usleep(DELAY);
//            while(waitKey(DELAY) != ' ');
//            usleep(100000);
//        }
        
        RhoPID.Update(&PID, actual, target);
        
//        printf("(%4d) actual:%3.6f target:%3.6f | ", i+1, actual, target);
//        RhoPID.Print(&PID);
//        printf("\n");
        
        actual -= PID.Value;
        
        a.x = x;
        a.y = FRAME_VERT_ZERO - actual*FRAME_VERT_SCALE;
//        frame.at<Vec3b>(a) = Vec3b(0,0,255);
        line(frame, a, b, Scalar(0,0,255));
        b = a;
        
        if(!(i%RPERIOD)) actual = RAND;
        
        imshow("PID Response", frame);
        
        x += FRAME_HORZ_SCALE;
        if(x >= frame.cols)
        {
            drawGrid(frame);
            x = 0;
            b = Point(FRAME_VERT_ZERO,0);
        }
        
        waitKey(DELAY);
    }

    return 0;
}
