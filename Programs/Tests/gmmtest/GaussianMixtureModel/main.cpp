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
#include <string.h>

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

MatrixXf Cholesky_Decomposition(MatrixXf matrix,
                            int n)
{
    std::cout << matrix(1,0) << std::endl;
    MatrixXf lower = MatrixXf::Zero(n,n);
    lower(0,0) = sqrt(matrix(0,0));
    lower(1,0) = matrix(1,0) / lower(0,0);
    lower(1,1) = sqrt( matrix(1,1) - lower(1,0) );
    
//    // Decomposing a matrix into Lower Triangular
//    for (int i = 0; i < 2; i++) {
//        for (int j = 0; j <= i; j++) {
//            double sum = 0;
//
//            if (j == i) // summation for diagonals
//            {
//                for (int k = 0; k < j; k++)
//                    sum += pow(lower(j,k), 2);
//                lower(j,j)= sqrt(matrix(j,j) - sum);
//            } else {
//
//                // Evaluating L(i, j) using L(j, j)
//                for (int k = 0; k < j; k++)
//                    sum += (lower(i,k) * lower(j,k));
//                lower(i,j) = (matrix(i,j)- sum) / lower(j,j);
//            }
//        }
//    }
//    lower(0,1) = matrix(0,1);
    return lower;
}

int main(int argc, const char * argv[])
{
    Eigen::MatrixXf A(2,2), B(2,2), C(2,2), D(2,2), E(2,2);
    A(0,0) = 10;
    A(0,1) = 1;
    A(1,0) = 3;
    A(1,1) = 9;
    Eigen::LLT<Eigen::MatrixXf> LLT;
    LLT.compute(A);
    B = LLT.solve(Eigen::MatrixXf::Identity(2, 2));
    
    double N = (LLT.matrixLLT().diagonal()).array().sum();
    std::cout << N << std::endl << std::endl;
    std::cout << A << std::endl << std::endl;
    std::cout << LLT.matrixLLT() << std::endl << std::endl;
    std::cout << B << std::endl << std::endl;
    
    C = Cholesky_Decomposition(A, 2);
    D = C * C.transpose();
    N = (C.diagonal()).array().sum();
    std::cout << N << std::endl << std::endl;
    std::cout << C << std::endl << std::endl;
    std::cout << D.inverse() << std::endl << std::endl;
    
    return 0;
}

int test()
{
    size_t s = 0;
    N = INC;

    do
    {
        dataMat = {0};
        
        Mat image = LoadImage("fade/1.png", N);
        vector<int> Dx = GenerateDx(image, data);
    
        int m = 0;
        for (int v : Dx)
        {
            if(v>m)m=v;
        }
        if(!m) continue;
        int x = 0;
        for (int v : Dx)
        {
            double c = 255*v/m;
            line(dataMat, Point(x,0), Point(x,HEIGHT), Scalar{0,c,0});
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
