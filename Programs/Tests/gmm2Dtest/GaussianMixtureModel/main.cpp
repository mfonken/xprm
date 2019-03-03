//
//  main.cpp
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 10/27/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include "data.h"
#include "image_handler.h"
#include "gaussianmixture.h"

using namespace std;
using namespace cv;

#define NUM_SAMPLES 10
#define SAMPLE_POINT_RADIUS 4

#define COV_SCALE 2

Mat gmmMat(HEIGHT, WIDTH, CV_8UC3, Scalar(245,245,245));
gaussian_mixture_model_t gmm;
GaussianMixture GMM(2, 2, INITIAL_VARIANCE, MAX_ERROR);

#define TMP(name, size) \
float *name##_data = (float *)(((intptr_t)alloca((size) * sizeof(float) + 16) + 16) & ~0xF); \
Eigen::Map<Eigen::VectorXf, Eigen::Aligned> name(name##_data, size);

static double GetCovarianceAngle( mat2x2 * covariance )
{
    double a = covariance->a, b = covariance->b, d = covariance->d,
    a_minus_d = a - d,
    radius = sqrt( a_minus_d * a_minus_d + 4. * b * b ),
    lambda = a_minus_d + radius;
    return atan2( -2 * b, a - lambda ) * 180 / M_PI;
}

int main(int argc, const char * argv[])
{
    Eigen::initParallel();
    
    gmmMat = Scalar(245,245,245);
    SampleList A_samples = GenerateNGaussian2DSamples( A_gaus, NUM_SAMPLES );
    SampleList B_samples = GenerateNGaussian2DSamples( B_gaus, NUM_SAMPLES );
    SampleList C_samples = GenerateNGaussian2DSamples( C_gaus, NUM_SAMPLES );
    PlotSamples(gmmMat, A_samples, SAMPLE_POINT_RADIUS, colors[1]);
    PlotSamples(gmmMat, B_samples, SAMPLE_POINT_RADIUS, colors[2]);
    PlotSamples(gmmMat, C_samples, SAMPLE_POINT_RADIUS, colors[4]);
    
    GMMFunctions.Model.Initialize( &gmm );
    
    SampleList all_samples[3] = {A_samples, B_samples, C_samples};
    uint8_t labels[3] = {A_label, B_label, C_label};
    vec2 centers[3] = { {750,250}, {500,500}, {250,750}};
    
    /******* A *******/
    int i = 0, l = 0;
    double angle;
    gaussian2d_t gaus;
    for( SampleList &sample_set : all_samples )
    {
        for( vec2& sample : sample_set )
        {
            observation_t observation = { sample.a/SCALE, sample.b/SCALE, labels[l] };
            vec2 value = { centers[l].a+5, centers[l].b+5 };
            
            printf("%d (%.2f, %.2f):\n", i++, value.a, value.b);
            GMMFunctions.Model.AddValue( &gmm, &observation, &value );
            
            VectorXf inputVec(2); inputVec << observation.a, observation.b;
            VectorXf valueVec(2); valueVec << sample.a/SCALE, sample.b/SCALE;
            GMM.setValue(inputVec, valueVec);
        }
        
        
        angle = GetCovarianceAngle( &gaus.covariance );
        ellipse(gmmMat, Point(gaus.mean.a*SCALE, gaus.mean.b*SCALE),
                Size(gaus.covariance.a*COV_SCALE*SCALE, gaus.covariance.d*COV_SCALE*SCALE), angle, 0, 360, Vec3b{0,105,0}, 3, 8);
        
        l++;
    }
    
    for( int j = 0; j < gmm.num_clusters; j++)
    {
        gaus = gmm.cluster[j].gaussian_out;
        angle = GetCovarianceAngle( &gaus.covariance );
        int b = 255-j*5, r =j*5;
        if( b<0) b=0; if( r>255) r=255;
        ellipse(gmmMat, Point(gaus.mean.a*SCALE, gaus.mean.b*SCALE),
                Size(gaus.covariance.a*COV_SCALE*SCALE, gaus.covariance.d*COV_SCALE*SCALE), angle, 0, 360, Vec3b{(uint8_t)b,0,(uint8_t)r}, 3, 8);
        
        printf("%d m_final: %.3f [%.2f %.2f | %.2f %.2f]\n", j, gmm.cluster[j].log_gaussian_norm_factor, gmm.cluster[j].llt_in.a, gmm.cluster[j].llt_in.b, gmm.cluster[j].llt_in.c, gmm.cluster[j].llt_in.d);
    }
    for( int j = 0; j < GMM.numClusters(); j++)
    {
        gaus.mean = (vec2){ GMM._neurons[j]->_mean_out[0], GMM._neurons[j]->_mean_out[1] };
        gaus.covariance = (mat2x2){
            GMM._neurons[j]->_covariance_in_out(0,0),
            GMM._neurons[j]->_covariance_in_out(0,1),
            GMM._neurons[j]->_covariance_in_out(1,0),
            GMM._neurons[j]->_covariance_in_out(1,1)
        };
        angle = GetCovarianceAngle( &gaus.covariance );
        int b = 255-j*5, r =j*5;
        if( b<0) b=0; if( r>255) r=255;
        ellipse(gmmMat, Point(gaus.mean.a*SCALE, gaus.mean.b*SCALE),
                Size(gaus.covariance.a*COV_SCALE*SCALE, gaus.covariance.d*COV_SCALE*SCALE), angle, 0, 360, Vec3b{0,205,0}, 3, 8);
        
        printf("%d o_final: %.3f [%.2f %.2f | %.2f %.2f]\n", j, GMM._neurons[j]->_log_input_gaussian_normalization, GMM._neurons[j]->_in_llt.matrixLLT()(0,0), GMM._neurons[j]->_in_llt.matrixLLT()(0,1), GMM._neurons[j]->_in_llt.matrixLLT()(1,0), GMM._neurons[j]->_in_llt.matrixLLT()(1,1));
    }
    usleep(100000);
    imshow("GMM", gmmMat);
    while(cv::waitKey(100) != ' ');
    
    return 0;
}

//
//#define VALUES 0,0,0,0 //4,2,2,1
////    double weight = 2;
//Eigen::MatrixXf _inv_covariance_in = [] {
//    Eigen::MatrixXf tmp(2,2);
//    tmp << VALUES;
//    return tmp;
//}();
//Eigen::LLT<Eigen::MatrixXf> _in_llt;
////    error = [] {
////        Eigen::Matrix2d tmp;
////        tmp << 1,0,0,1;
////        return tmp;
////    }();
////    VectorXf a(2), b(2);
////    a[0] = -1; a[1] = -2; b[0] = 3; b[1] = 4;
////    TMP(delta_mean_in, 2);
//////    TMP(temp, 2);
////    delta_mean_in = a - b;
////    cout << delta_mean_in << endl << endl;
////    auto temp = delta_mean_in * delta_mean_in.transpose();
////    cout << temp << endl;
//////    _covariance_in += weight * (delta_mean_in * delta_mean_in.transpose() + error - _covariance_in);
////
//_in_llt.compute(_inv_covariance_in);
//auto llt = _in_llt.matrixLLT();
//cout << (M_PI*M_PI * llt.diagonal()).array().cwiseSqrt() << endl<<endl;
//auto out = -1.0f * (M_PI*M_PI * llt.diagonal()).array().cwiseSqrt().log().sum();
//cout << out << endl<<endl;
////
////    mat2x2 llt_pre = { VALUES }, llt_in;
////    GetMat2x2LLT( &llt_pre, &llt_in );
////    double cholesky_dms = llt_in.a * llt_in.d, norm_factor = -(double)(ZDIV_LNUM);
////    if( cholesky_dms ) norm_factor = -log( M_PI * M_PI * sqrt( cholesky_dms ) );
////    cout << norm_factor << endl<<endl;
//
////    VectorXf input(2), _mean_in(2);
////    input[0] = -1; input[1] = -2; _mean_in[0] = 300; _mean_in[1] = 4;
////    TMP(delta, 2);
////    TMP(cov_times_delta, 2);
////
////    delta = input - _mean_in;
////
////    cout << delta << endl<<endl;
////
////    cov_times_delta.setZero();
////    cov_times_delta.noalias() += _inv_covariance_in * delta;
////
////    cout << cov_times_delta << endl<<endl;
////
////    // Compute the normal distribution and mahalanobis distance
////    double _square_mahalanobis_distance = delta.dot(cov_times_delta);
////    cout << _square_mahalanobis_distance << endl<<endl;
////    auto _probability_of_in = safe_exp(
////      _log_input_gaussian_normalization - 0.5 * _square_mahalanobis_distance
////      );
////    cout << _probability_of_in << endl<<endl;
////
////    vec2 a = { input[0], input[1] }, b = {_mean_in[0], _mean_in[1] },
////    input_delta = { a.a - b.a, a.b - b.b };
////    mat2x2 ici = { VALUES };
////    double msq = CalculateMahalanobisDistanceSquared( &ici, &input_delta);
////
////    _in_llt.compute(_covariance_in);
////    cout << _in_llt.matrixLLT() <<endl<<endl;
////    auto _inv_covariance_in = _in_llt.solve(Eigen::MatrixXf::Identity(2, 2));
////    cout << _inv_covariance_in <<endl<<endl;
////
////    mat2x2 cov = { VALUES }, inv;
//////    GetMat2x2LLT( &cov, &LLT );
//////    vec2 v;
//////    CalculateMahalanobisDistanceSquared( &LLT, &v);
////    getMat2x2Inverse( &cov, &inv );
////    while(1);

