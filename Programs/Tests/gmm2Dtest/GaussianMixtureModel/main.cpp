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

//#define SPOOF

#define NUM_SAMPLES 100
#define SAMPLE_POINT_RADIUS 4
#define NUM_LABELS_TO_SHOW 4

#define COV_SCALE VALID_CLUSTER_STD_DEV

Mat gmmMat(2*BORDER+HEIGHT, 2*BORDER+WIDTH, CV_8UC3, Scalar(245,245,245));
gaussian_mixture_model_t * gmm;
psm_t psm;
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

static void PostProcess()
{
    PSMFunctions.Update( &psm, 20. );
    for( uint8_t i = 0; i < psm.state_bands.length; i++ )
    {
        double l = psm.state_bands.band[i].lower_boundary, u = psm.state_bands.band[i].upper_boundary;
        vec2 tc = psm.state_bands.band[i].true_center;
        if(tc.a == 0 && tc.b == 0) tc = (vec2){0, HEIGHT};
        int tc_line_len = 20;
        
        double f = ((double)i)/5.;
        Vec3b c(255*(1-f),0,255*f);
        line(gmmMat, Point(BORDER, l+BORDER), Point(WIDTH+BORDER, l+BORDER), c);
        
        line(gmmMat, Point(tc.a+BORDER-tc_line_len, tc.b+BORDER), Point(tc.a+BORDER+tc_line_len, tc.b+BORDER), c, 3);
        line(gmmMat, Point(tc.a+BORDER, tc.b+BORDER-tc_line_len), Point(tc.a+BORDER, tc.b+BORDER+tc_line_len), c, 3);
        
        rectangle(gmmMat, Point(0, l+BORDER), Point(BORDER, u+BORDER), c, FILLED);
        rectangle(gmmMat, Point(WIDTH+BORDER, l+BORDER), Point(WIDTH+BORDER*2, u+BORDER), c, FILLED);
        putText(gmmMat, to_string(i), Point(BORDER+12, l+BORDER-4), FONT_HERSHEY_PLAIN, 1, c, 2);
    }
}

int main(int argc, const char * argv[])
{
//    Eigen::initParallel();
    
    gmmMat = Scalar(245,245,245);
    rectangle(gmmMat, Point(BORDER,BORDER), Point(BORDER+WIDTH,BORDER+HEIGHT), {0});
    SampleList A_samples = GenerateNGaussian2DSamples( A_gaus, NUM_SAMPLES, 0);
#ifdef SEPARATE_IDS
    SampleList B_samples = GenerateNGaussian2DSamples( B_gaus, NUM_SAMPLES, 1);
    SampleList C_samples = GenerateNGaussian2DSamples( C_gaus, NUM_SAMPLES, 2);
#endif
    PlotSamples(gmmMat, A_samples, SAMPLE_POINT_RADIUS, colors[0]);
#ifdef SEPARATE_IDS
    PlotSamples(gmmMat, B_samples, SAMPLE_POINT_RADIUS, colors[1]);
    PlotSamples(gmmMat, C_samples, SAMPLE_POINT_RADIUS, colors[2]);
#endif
    
//    GMMFunctions.Model.Initialize( &gmm );
    PSMFunctions.Initialize( &psm );
    gmm = &psm.gmm;
    
    SampleList all_samples[] = {A_samples
#ifdef SEPARATE_IDS
        , B_samples, C_samples
#endif
    };
//    uint8_t labels[3] = {A_label, B_label, C_label};
#define NUM_SNAP 10
    int centers[NUM_SNAP];
    for(int i = 0; i <= NUM_SNAP; i++)
        centers[i] = (i+1) * HEIGHT/(NUM_SNAP+1);
    
    double angle;
    gaussian2d_t gaus = {0};
#ifdef SPOOF
    /* Cluster Spoof */
#define COV_X 50
#define COV_XY 0
#define COV_Y 100
    psm.gmm.num_clusters = 12;
    
    uint8_t labels[4][4] =
    {
        {0,0,0,0},
        {0,0,0,0},
        {3,3,2,2},
        {1,0,2,2},
    };
    for( uint8_t i = 0; i < psm.gmm.num_clusters; i++ )
    {
        double x = 100*(i+1)-200, y = HEIGHT-x+100;
        psm.gmm.cluster_mem[i].gaussian_in = { {x,y}, {COV_X,COV_XY,COV_XY,COV_Y} };
        psm.gmm.cluster_mem[i].max_y = x + COV_Y*VALID_CLUSTER_STD_DEV;
        for(uint8_t j = 0; j < 4; j++)
            ReportLabel( &psm.gmm.cluster_mem[i].labels, labels[i][j] );
    }
    mulGaussian2d( &psm.gmm.cluster_mem[0].gaussian_in, &psm.gmm.cluster_mem[1].gaussian_in, &psm.gmm.cluster_mem[psm.gmm.num_clusters++].gaussian_in );
    for(uint8_t i = 2; i < 12; i++)
        mulGaussian2d( &psm.gmm.cluster_mem[psm.gmm.num_clusters-1].gaussian_in, &psm.gmm.cluster_mem[i].gaussian_in, &psm.gmm.cluster_mem[psm.gmm.num_clusters++].gaussian_in );
    
    /*****************/
#else
    /******* A *******/
    int l = 0, i = 0;
    for( SampleList &sample_set : all_samples )
    {
//        for( vec2& sample : sample_set )
//        {
        for(int j = (int)sample_set.size()-1; j >= 0; j--)
        {
            dm_line &sample = sample_set[j];
            observation_t observation = { (double)sample.density/SCALE, (double)sample.thresh/SCALE, (uint8_t)sample.id_ };
//            uint8_t k = (uint8_t)(observation.b / HEIGHT/(NUM_SNAP+1));
            vec2 value = { observation.a, observation.b };//(double)centers[k] };// };
            
            printf("%d (%.2f, %.2f):\n", i++, value.a, value.b);
//            GMMFunctions.Model.AddValue( &gmm, &observation, &value );
            PSMFunctions.ReportObservations( &psm, &observation, 1 );
            
            VectorXf inputVec(2); inputVec << observation.a, observation.b;
            VectorXf valueVec(2); valueVec << observation.a, observation.b;
            GMM.setValue(inputVec, valueVec);
        }
        
        
        angle = GetCovarianceAngle( &gaus.covariance );
        ellipse(gmmMat, Point(gaus.mean.a*SCALE+BORDER, gaus.mean.b*SCALE+BORDER),
                Size(gaus.covariance.a*COV_SCALE*SCALE, gaus.covariance.d*COV_SCALE*SCALE), angle, 0, 360, Vec3b{0,105,0}, 3, 8);
        
        l++;
    }
#endif
    for( int j = 0; j < gmm->num_clusters; j++)
    {
//        printf("%d m_final: %.3f [%.2f %.2f | %.2f %.2f]\n", j, gmm->cluster[j]->log_gaussian_norm_factor, gmm->cluster[j]->llt_in.a, gmm->cluster[j]->llt_in.b, gmm->cluster[j]->llt_in.c, gmm->cluster[j]->llt_in.d);
#ifdef SHOW_CLUSTERS
        gaus = (*gmm->cluster[j]).gaussian_in;
        angle = GetCovarianceAngle( &gaus.covariance );
        int b = 255-j*5, r =j*5;
        if( b<0) b=0; if( r>255) r=255;
        Point center = Point(gaus.mean.a*SCALE+BORDER, gaus.mean.b*SCALE+BORDER);
        double cs = pow(gaus.combinations+1,sqrt(M_PI)); // Combination scale
        Size size = Size(gaus.covariance.a*COV_SCALE*SCALE*cs, gaus.covariance.d*COV_SCALE*SCALE*cs);
        ellipse(gmmMat, center, size, angle, 0, 360, Vec3b{(uint8_t)b,0,(uint8_t)r}, 1, 5, 0);
        label_manager_t &labels = (*gmm->cluster[j]).labels;
        for(int i = 0, offset = -12; i < NUM_LABELS_TO_SHOW; i++, offset+=12)
        {
            Scalar c(colors[i][0]*0.7,colors[i][1]*0.7,colors[i][2]*0.7);
            putText(gmmMat, pto_string(labels.average[i],1), Point(center.x, center.y+offset), FONT_HERSHEY_PLAIN, 1, c, 2);
        }
#endif
    }
    for( int j = 0; j < GMM._neurons.size(); j++)
    {
//        printf("%d m_final: %.3f [%.2f %.2f | %.2f %.2f]\n", j, (*gmm->cluster[j]).log_gaussian_norm_factor, gmm->cluster[j]->llt_in.a, gmm->cluster[j]->llt_in.b, gmm->cluster[j]->llt_in.c, gmm->cluster[j]->llt_in.d);
#ifdef SHOW_CLUSTERS
        gaus =
        {
            {
                GMM._neurons[j]->_mean_in[0],
                GMM._neurons[j]->_mean_in[1],
            },
            {
                GMM._neurons[j]->_covariance_in(0,0),
                GMM._neurons[j]->_covariance_in(0,1),
                GMM._neurons[j]->_covariance_in(1,0),
                GMM._neurons[j]->_covariance_in(1,1)
            }
        };
        angle = GetCovarianceAngle( &gaus.covariance );
        int b = 255-j*5, r =j*5;
        if( b<0) b=0; if( r>255) r=255;
        Point center = Point(gaus.mean.a*SCALE+BORDER, gaus.mean.b*SCALE+BORDER);
        Size size = Size(gaus.covariance.a*COV_SCALE*SCALE, gaus.covariance.d*COV_SCALE*SCALE);
//        ellipse(gmmMat, center, size, angle, 0, 360, Vec3b{(uint8_t)b,0,(uint8_t)r}, 1, 5, 0);
        label_manager_t &labels = gmm->cluster[j]->labels;
        for(int i = 0, offset = -12; i < 3; i++, offset+=12)
        {
            Scalar c(colors[i][0]*0.7,colors[i][1]*0.7,colors[i][2]*0.7);
            putText(gmmMat, pto_string(labels.average[i],1), Point(center.x, center.y+offset), FONT_HERSHEY_PLAIN, 1, c, 2);
        }
#endif
    }
    while(1)
    {
    usleep(100000);
        imshow("GMM", gmmMat);
#ifdef SAVE_TO_FILE
        string fileName(string(ROOT_PATH) + this_descriptor
#ifdef SHOW_CLUSTERS
        + "_clusters"
#endif
        + ".png");
        imwrite(fileName, gmmMat);
#endif
        
        while(cv::waitKey(100) != ' ');
        
        PostProcess();
    }
    return 0;
}
