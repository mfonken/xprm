//
//  main.cpp
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 10/27/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include "data.h"
#include "gaussianmixture.h"
#include "image_handler.h"

using namespace Eigen;
using namespace std;
using namespace cv;

#define INITIAL_VARIANCE    0.1
#define MAX_ERROR           0.1

#define DIMI 1
#define DIMO DIMI

#define INC 20

Mat gmmMat(HEIGHT, WIDTH, CV_8UC3, Scalar(245,245,245));

IOFormat fmt(3, DontAlignCols, "\t", ", ", "", "", "", "");

int main(int argc, const char * argv[])
{
    size_t s = 0;
    N = INC;

    do
    {
        dataMat = {0};
        
        Mat image = LoadImage("frames/ellipse/1.png", N);
        vector<int> Dx = GenerateDx(image, data);
    
        int m = 0;
        for (int v : Dx)
        {
            if(v>m)m=v;
        }
        int x = 0;
        for (int v : Dx)
        {
            double c = 255*v/m;
            line(dataMat, Point(x,0), Point(x,HEIGHT), Scalar{0,c,c});
            data[x] = (double)v/m*50;
            x++;//=WIDTH/Dx.size();
        }
        
//        GenerateRandomData(N, K, W);
        GaussianMixture gmm(DIMI,DIMO,INITIAL_VARIANCE*N,MAX_ERROR);
        vector<VectorXf> iData, oData;
        for(int i = 0; i < N; i++)
        {
            VectorXf iv(DIMI);
            iv << data[i];
            iData.push_back(iv);
            
            VectorXf ov(DIMO);
            ov << 0;
            oData.push_back(ov);
            
            gmm.setValue(iv, ov);
        }
        
        s = gmm.numClusters();
        int prev_x = 0;
        cout << N << ":" << endl;
        for(size_t i = 0; i < s; i++)
        {
            GaussianMixture::Neuron n = *gmm._neurons[i];
            int x = n._mean_in[0];///DATA_LEN * WIDTH;
            if(x < 10){ s--; continue;}
            if(x == prev_x) {s--;continue;}
            
            Scalar color(0,0,255);
            line(dataMat, Point(x-1,0), Point(x-1,HEIGHT), color);
            line(dataMat, Point(x,0), Point(x,HEIGHT), color);
            line(dataMat, Point(x+1,0), Point(x+1,HEIGHT), color);
            
            cout << i << ": " << n.ToString() << endl;
            prev_x = x;
        }
        memset(data, 0, sizeof(data[0])*N);
        N += INC;
        line(dataMat, Point(N,0), Point(N,HEIGHT),Scalar{255,255,255});
        imshow("Data", dataMat);
//        imshow("Image", image);
//        imshow("Dx", dxmat);
        char c = waitKey(100);
        if(c == ' ')
        {
            usleep(1000000);
            waitKey(10000);
        }
    } while( N < WIDTH );//s < K );
    
    printf("Set size is %d\n", N);
    imshow("Data", dataMat);
    waitKey();

    return 0;
}
