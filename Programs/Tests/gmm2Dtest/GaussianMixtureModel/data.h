//
//  data.h
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 11/3/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#ifndef data_h
#define data_h

#include <stdio.h>
#include <stdlib.h>     /* srand, rand */

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <unistd.h>

using namespace cv;

#define DATA_LEN 10000
#define HEIGHT_ABS 100
#define WIDTH_ABS 100
#define SCALE 10
#define WIDTH WIDTH_ABS * SCALE
#define HEIGHT HEIGHT_ABS * SCALE

static int N = DATA_LEN;
static int K = 6;
static int W = (0.5 * N/K);

#define DIV 1000

static Scalar colors[6] =
{
    {255,0,0},
    {255,255,0},
    {0,255,0},
    {0,255,255},
    {0,0,255},
    {255,0,255}
};

static double data[DATA_LEN];
Mat dataMat(HEIGHT, WIDTH, CV_8UC3, Scalar(0,0,0));

static void GenerateRandomData(int num, int regions, int space, int min = 0, int max = DATA_LEN)
{
    int range = max - min;
    int split = range/(regions);
    int width = split - space;
    int num_per_region = num / regions;
    
    int index = 0;
    int prev = 0;
    for(int i = 0; i < regions; i++)
    {
        int region_center = i * split + (split - width)/2;
        for(int j = 0; j < num_per_region; j++)
        {
            int random_v = rand() % (int)(width * DIV);
            double v = (double)(random_v) / DIV;
            data[index] = v + region_center;
            
            double curr_v = data[index];
            int x = (int)((curr_v / (double)range) * (double)WIDTH);
            if(x == prev) continue;
            line(dataMat, Point(x,0), Point(x,HEIGHT), colors[i]);
            x = prev;
            
            index++;
        }
    }
    std::sort(data, data + DATA_LEN);
}

#endif /* data_h */
