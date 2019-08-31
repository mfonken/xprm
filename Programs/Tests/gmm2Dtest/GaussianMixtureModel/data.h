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
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <Eigen/Dense>
#include <random>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <unistd.h>

#include "psm.h"

using namespace cv;
using namespace std;

//#define SEPARATE_IDS
#define SHOW_CLUSTERS
#define REPORT_LABELS
#define SAVE_TO_FILE

#define ROOT_PATH "/Users/matthew/Desktop/gmmtest/data/"
#define DETECTION_MAP_FILE ROOT_PATH"dm.txt"

#define DATA_LEN 10000
#define HEIGHT 700
#define WIDTH 700
#define SCALE 1
#define BORDER 50//100

#define NUM_SAMPLES 100
#define SAMPLE_POINT_RADIUS 4
#define NUM_LABELS_TO_SHOW 4

typedef struct
{
    uint32_t
//    data_len,
    height,
    width,
    scale,
    border,
    num_samples,
    point_radius;
} global_settings_t;

static global_settings_t settings =
{
    //    DATA_LEN,
    HEIGHT,
    WIDTH,
    SCALE,
    BORDER,
    NUM_SAMPLES,
    SAMPLE_POINT_RADIUS
};

static string this_descriptor = "test";

typedef struct
{
    int density, thresh, id_;
} dm_line;

static int N = DATA_LEN;
static int K = 6;
static int W = (0.5 * N/K);

#define DIV 1000

using namespace std;
using namespace Eigen;
using namespace cv;

typedef Vec3b Color;
typedef vector<dm_line> SampleList;

static Color colors[6] =
{
    {255,255,0},
    {0,255,0},
    {0,0,255},
    {255,0,0},
    {0,255,255},
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
            int x = (int)((curr_v / (double)range) * (double)settings.width);
            if( x == prev ) continue;
            line(dataMat, Point(x,0), Point(x,settings.height), colors[i]);
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

static uint8_t A_label = 0;
static int A_var = 200;
static int A_co = 40;
static Vector2d A_mean(770,100);
static Matrix2d A_cov = [] {
    Matrix2d tmp;
    tmp << A_var,A_co,A_co,A_var;
    return tmp;
    }();
static normal_random_variable A_gaus(A_mean, A_cov);

static uint8_t B_label = 1;
static int B_var = 700;
static int B_co = 400;
static Vector2d B_mean(850,300);
static Matrix2d B_cov = [] {
    Matrix2d tmp;
    tmp << B_var,B_co,B_co,B_var;
    return tmp;
}();
static normal_random_variable B_gaus(B_mean, B_cov);

static uint8_t C_label = 2;
static int C_var = 700;
static int C_co = -100;
static Vector2d C_mean(250,750);
static Matrix2d C_cov = [] {
    Matrix2d tmp;
    tmp << C_var,C_co,C_co,C_var;
    return tmp;
}();
static normal_random_variable C_gaus(C_mean, C_cov);

static bool is_int(const string s){
    return s.find_first_not_of( "0123456789" ) == string::npos;
}

bool compareVecB(dm_line v1, dm_line v2)
{
    return (v1.thresh > v2.thresh);
}

static int label = 0;
static SampleList GenerateNGaussian2DSamples( normal_random_variable nvar, int n, int id_ = 0 )
{
    SampleList samples;
#ifdef DETECTION_MAP_FILE
    ifstream file(DETECTION_MAP_FILE);
    string line;
    while ( getline(file,line) )
    {
        stringstream ss(line);
        vector<string> result;
        while( ss.good() )
        {
            string substr;
            getline( ss, substr, ',' );
            result.push_back( substr );
        }
        if( is_int(result[0])
#ifdef SEPARATE_IDS
           && (int)stoi(result[2]) == label
#endif
           )
        {
            int x = stoi(result[0])*2,
            y = stoi(result[1])*settings.height/256,
            _id = stoi(result[2]);
            
            samples.push_back( (dm_line){ x, y, _id } );
//            printf("%lu %d %d\n", samples.size(), x, y);
        }
        else if(result.size()==1)
        {
            this_descriptor = result[0];
        }
    }

#else
    for( int i = 0; i < n; i++ )
    {
        Vector2d s = nvar();
        samples.push_back( (dm_line){ (int)s[0], (int)s[1], id_ });
    }
#endif
    label++;
//    sort(samples.begin(), samples.end(), compareVecB);
    return samples;
}

static void PlotSamples( Mat &M, SampleList l, int r, Color &c )
{
    for( dm_line sample : l )
        circle(M, Point(sample.density + settings.border,sample.thresh + settings.border), r, colors[sample.id_], FILLED);
}

template <typename T>
std::string pto_string(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

#endif /* data_h */
