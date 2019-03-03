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
#include <Eigen/Dense>
#include <random>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <unistd.h>

#include "gmm.h"

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

using namespace std;
using namespace Eigen;
using namespace cv;

typedef Vec3b Color;
typedef vector<vec2> SampleList;
typedef observation_t * ObservationList;

static Color colors[6] =
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

struct normal_random_variable
{
    normal_random_variable(Eigen::MatrixXd const& covar)
    : normal_random_variable(Eigen::VectorXd::Zero(covar.rows()), covar)
    {}
    
    normal_random_variable(Eigen::VectorXd const& mean, Eigen::MatrixXd const& covar)
    : mean(mean)
    {
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(covar);
        transform = eigenSolver.eigenvectors() * eigenSolver.eigenvalues().cwiseSqrt().asDiagonal();
    }
    
    Eigen::VectorXd mean;
    Eigen::MatrixXd transform;
    
    Eigen::VectorXd operator()() const
    {
        static std::mt19937 gen{ std::random_device{}() };
        static std::normal_distribution<> dist;
        
        return mean + transform * Eigen::VectorXd{ mean.size() }.unaryExpr([&](auto x) { return dist(gen); });
    }
};

static uint8_t A_label = 'A';
static int A_var = 200;
static int A_co = 40;
static Vector2d A_mean(500,500);
static Matrix2d A_cov = [] {
    Matrix2d tmp;
    tmp << A_var,A_co,A_co,A_var;
    return tmp;
    }();
static normal_random_variable A_gaus(A_mean, A_cov);

static uint8_t B_label = 'B';
static int B_var = 300;
static int B_co = 100;
static Vector2d B_mean(750,250);
static Matrix2d B_cov = [] {
    Matrix2d tmp;
    tmp << B_var,B_co,B_co,B_var;
    return tmp;
}();
static normal_random_variable B_gaus(B_mean, B_cov);

static uint8_t C_label = 'C';
static int C_var = 300;
static int C_co = -100;
static Vector2d C_mean(250,750);
static Matrix2d C_cov = [] {
    Matrix2d tmp;
    tmp << C_var,C_co,C_co,C_var;
    return tmp;
}();
static normal_random_variable C_gaus(C_mean, C_cov);

static SampleList GenerateNGaussian2DSamples( normal_random_variable nvar, int n )
{
    SampleList samples;
    
    for( int i = 0; i < n; i++ )
    {
        Vector2d s = nvar();
//        printf("%3d>(%.2f, %.2f)\n", i, s[0], s[1]);
        samples.push_back( (vec2){ s[0], s[1] });
    }
    return samples;
}

static void PlotSamples( Mat &M, SampleList l, int r, Color &c )
{
    for( vec2 sample : l )
        circle(M, Point(sample.a,sample.b), r, c, FILLED);
}

static void ConvertSampleListToObservationList( SampleList &sl , ObservationList &ol, uint8_t label )
{
    int i = 0;
    for( vec2 &sample : sl )
    {
        ol[i++] = (observation_t){ sample.a, sample.b, label };
    }
}
#endif /* data_h */
