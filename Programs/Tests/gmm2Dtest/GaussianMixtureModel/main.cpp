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

//#define USE_CPP_VERSION

#define data_len_max 1000000
#define height_max 2000
#define width_max height_max
#define scale_max 100
#define border_max 1000
#define num_samples_max 10000
#define point_radius_max 10
bool states = false;

gaussian_mixture_settings_t gmm_settings;

const global_settings_t settings_max =
{
    //    data_len_max,
    height_max,
    width_max,
    scale_max,
    border_max,
    num_samples_max,
    point_radius_max
};

const global_settings_t settings_step =
{
    100,
    100,
    1,
    25,
    100,
    1
};

const char * mainMatTitle = "GMM";


global_settings_t settings_scaled;

const uint num_settings = sizeof(settings)/sizeof(uint32_t);

static char settings_bars_names[num_settings][50]
{
    //    "Data Length",
    "Height",
    "Size",
    "Scale",
    "Border",
    "Samples",
    "Point r"
};

//#define SPOOF

#define COV_SCALE VALID_CLUSTER_STD_DEV

Mat gmmMat(2*BORDER+HEIGHT, 2*BORDER+WIDTH, CV_8UC3, Scalar(245,245,245));

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

static void PostProcess(psm_t * psm)
{
    PSMFunctions.Update( psm, 20. );
    for( uint8_t i = 0; i < psm->state_bands.length; i++ )
    {
        band_t * band = &psm->state_bands.band[i];
        double l = band->lower_boundary, u = band->upper_boundary;
        vec2 tc = band->true_center;
        if(tc.a == 0 && tc.b == 0) tc = (vec2){0, (double)settings.height};
        int tc_line_len = 20;
        
        double f = ((double)i)/5.;
        Vec3b c(255*(1-f),0,255*f);
        line(gmmMat, Point(settings.border, l+settings.border), Point(settings.width+settings.border, l+settings.border), c);
        
        line(gmmMat, Point(tc.a+settings.border-tc_line_len, tc.b+settings.border), Point(tc.a+settings.border+tc_line_len, tc.b+settings.border), c, 3);
        line(gmmMat, Point(tc.a+settings.border, tc.b+settings.border-tc_line_len), Point(tc.a+settings.border, tc.b+settings.border+tc_line_len), c, 3);
        
        rectangle(gmmMat, Point(0, l+settings.border), Point(settings.border, u+settings.border), c, FILLED);
        rectangle(gmmMat, Point(settings.width+settings.border, l+settings.border), Point(settings.width+settings.border*2, u+settings.border), c, FILLED);
        putText(gmmMat, to_string(i), Point(settings.border+12, l+settings.border-4), FONT_HERSHEY_PLAIN, 1, c, 2);
        
        /*****************/
        if(band->variance > 0)
        {
            double prev_x = 0, variance_scale = 100;//ZDIV( 1, band->variance * M_PI * 2 );
            for(uint16_t y = 1; y < settings.height; y++)
            {
                double upper = y - band->true_center.b;
                double inner = ZDIV( upper, band->variance );
                double exponent = -0.5 * inner * inner;
                double x = variance_scale * safe_exp(exponent);
                x = MAX(0, x);
                x = MIN(settings.width, x);
                Point p(settings.width + settings.border - x, settings.border+y);
//                gmmMat.at<Vec3b>(settings.border+y,settings.width + settings.border - x) = colors[i];
                line( gmmMat, Point(prev_x, p.y-1), p, colors[i] );
                prev_x = p.x;
            }
        }
        /*****************/
//        psm->state_bands.band[0].
    }
}

void run()
{
    gaussian_mixture_model_t * gmm;
    psm_t psm;
    
#ifdef USE_CPP_VERSION
    GaussianMixture GMM(2, 2, INITIAL_VARIANCE, MAX_ERROR, gmm_settings.value);
#endif
    
    gmmMat = Scalar(245,245,245);
    rectangle(gmmMat, Point(settings.border,settings.border), Point(settings.border+settings.width,settings.border+settings.height), {0});
    SampleList A_samples = GenerateNGaussian2DSamples( A_gaus, settings.num_samples, 0);
#ifdef SEPARATE_IDS
    SampleList B_samples = GenerateNGaussian2DSamples( B_gaus, settings.num_samples, 1);
    SampleList C_samples = GenerateNGaussian2DSamples( C_gaus, settings.num_samples, 2);
#endif
    PlotSamples(gmmMat, A_samples, settings.point_radius, colors[0]);
#ifdef SEPARATE_IDS
    PlotSamples(gmmMat, B_samples, settings.point_radius, colors[1]);
    PlotSamples(gmmMat, C_samples, settings.point_radius, colors[2]);
#endif
    
//    GMMFunctions.Model.Initialize( &gmm );
    PSMFunctions.Initialize( &psm );
    gmm = &psm.gmm;
    
    memcpy(&gmm->settings, &gmm_settings, sizeof(gaussian_mixture_settings_t));
    
    SampleList all_samples[] = {A_samples
#ifdef SEPARATE_IDS
        , B_samples, C_samples
#endif
    };
//    uint8_t labels[3] = {A_label, B_label, C_label};
#define NUM_SNAP 10
    int centers[NUM_SNAP];
    for(int i = 0; i <= NUM_SNAP; i++)
        centers[i] = (i+1) * settings.height/(NUM_SNAP+1);
    
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
        double x = 100*(i+1)-200, y = settings.height-x+100;
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
            observation_t observation = { (double)sample.density/settings.scale, (double)sample.thresh/settings.scale, (uint8_t)sample.id_ };
//            uint8_t k = (uint8_t)(observation.b / settings.height/(NUM_SNAP+1));
            vec2 value = { observation.a, observation.b };//(double)centers[k] };// };
            
            
            //printf("%d (%.2f, %.2f):\n", i++, value.a, value.b);
            
            
//            GMMFunctions.Model.AddValue( &gmm, &observation, &value );
            PSMFunctions.ReportObservations( &psm, &observation, 1 );
            
            VectorXf inputVec(2); inputVec << observation.a, observation.b;
            VectorXf valueVec(2); valueVec << observation.a, observation.b;
#ifdef USE_CPP_VERSION
            GMM.setValue(inputVec, valueVec);
#endif
        }
        
        
        angle = GetCovarianceAngle( &gaus.covariance );
        ellipse(gmmMat, Point(gaus.mean.a*settings.scale+settings.border, gaus.mean.b*settings.scale+settings.border),
                Size(gaus.covariance.a*COV_SCALE*settings.scale, gaus.covariance.d*COV_SCALE*settings.scale), angle, 0, 360, Vec3b{0,105,0}, 3, 8);
        
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
        Point center = Point(gaus.mean.a*settings.scale+settings.border, gaus.mean.b*settings.scale+settings.border);
        double cs = pow(gaus.combinations+1,sqrt(M_PI)); // Combination scale
        Size size = Size(gaus.covariance.a*COV_SCALE*settings.scale*cs, gaus.covariance.d*COV_SCALE*settings.scale*cs);
        ellipse(gmmMat, center, size, angle, 0, 360, Vec3b{(uint8_t)b,0,(uint8_t)r}, 1, 5, 0);
        label_manager_t &labels = (*gmm->cluster[j]).labels;
#ifdef REPORT_LABELS
        for(int i = 0, offset = -12; i < NUM_LABELS_TO_SHOW; i++, offset+=12)
        {
            Scalar c(colors[i][0]*0.7,colors[i][1]*0.7,colors[i][2]*0.7);
            putText(gmmMat, pto_string(labels.average[i],1), Point(center.x, center.y+offset), FONT_HERSHEY_PLAIN, 1, c, 2);
        }
#endif
#endif
    }
    
#ifdef USE_CPP_VERSION
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
        Point center = Point(gaus.mean.a*settings.scale+settings.border, gaus.mean.b*settings.scale+settings.border);
        Size size = Size(gaus.covariance.a*COV_SCALE*settings.scale, gaus.covariance.d*COV_SCALE*settings.scale);
        ellipse(gmmMat, center, size, angle, 0, 360, Vec3b{(uint8_t)b,0,(uint8_t)r}, 1, 5, 0);
#endif
    }
#endif
    
    if(states)
    {
        PostProcess(&psm);
    }
    return;
}

const string settingsGuiTitle = "Settings";
Mat settingsGuiMat(1, 600, CV_8UC3, Scalar(245,245,245));

static void on_settings_trackbar( int, void* )
{
    imshow(settingsGuiTitle, settingsGuiMat);
}

void CreateSettingsGUI()
{
    namedWindow(settingsGuiTitle, WINDOW_AUTOSIZE);
    
    for( uint8_t i = 1; i < num_settings; i++ )
    {
        double scale = (double)((uint32_t *)&settings_step)[i];
        uint32_t max_value = (uint32_t)((double)((uint32_t *)&settings_max)[i]/scale);
        ((uint32_t *)&settings_scaled)[i] = (uint32_t)((double)((uint32_t *)&settings)[i]/scale);
        int * pValue = (int *)&((uint32_t *)&settings_scaled)[i];
        cv::createTrackbar( settings_bars_names[i], settingsGuiTitle, pValue, max_value, on_settings_trackbar );
    }
}

void ApplySettings()
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("Global Settings:\n");
    for( uint8_t i = 1; i < num_settings; i++ )
    {
        double scale = (double)((uint32_t *)&settings_step)[i];
        ((uint32_t *)&settings)[i] = (uint32_t)((double)((uint32_t *)&settings_scaled)[i]*scale);
        
        printf(" - %s: %d\n", settings_bars_names[i], ((uint32_t *)&settings)[i]);
    }
    settings.height = settings.width;
    resize(gmmMat, gmmMat, Size(2*settings.border+settings.width, 2*settings.border+settings.height));
    
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
}

const string gmmSettingsGuiTitle = "GMM Settings";
Mat gmmSettingsGuiMat(1, 600, CV_8UC3, Scalar(245,245,245));

static void on_gmm_settings_trackbar( int, void* )
{
    imshow(gmmSettingsGuiTitle, gmmSettingsGuiMat);
}

void CreateGMMSettingsGUI()
{
    gaussian_mixture_model_t gmm;
    GMMFunctions.Model.Initialize(&gmm);
    memcpy(&gmm_settings, &gmm.settings, sizeof(gaussian_mixture_settings_t));
    
    
    namedWindow(gmmSettingsGuiTitle, WINDOW_AUTOSIZE);
    
    for( uint8_t i = 0; i < gmm_settings.num; i++ )
    {
        double scale = ((double *)&gmm_settings.step)[i];
        uint32_t max_value = (uint32_t)(((double *)&gmm_settings.max)[i]/scale);
        ((uint32_t *)&gmm_settings.scaled)[i] = (uint32_t)(((double *)&gmm_settings.value)[i]/scale);
        cv::createTrackbar( gmm_settings_names[i], gmmSettingsGuiTitle, &gmm_settings.scaled[i], max_value, on_gmm_settings_trackbar );
    }
}

void ApplyGMMSettings()
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("GMM Settings:\n");
    for(uint8_t i = 0; i < gmm_settings.num; i++)
    {
        double scale = ((double *)&gmm_settings.step)[i];
        ((double *)&gmm_settings.value)[i] = (double)gmm_settings.scaled[i]*scale;
        
        printf(" - %s: %.4f\n", gmm_settings_names[i], ((double *)&gmm_settings.value)[i]);
    }
    gmm_settings.value.inv_initial_variance = ZDIV( 1., gmm_settings.value.initial_variance );
    gmm_settings.value.max_mahalanobis_sq_for_update = gmm_settings.value.max_mahalanobis_sq;
    
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
}

int main(int argc, const char * argv[])
{
    CreateSettingsGUI();
    on_settings_trackbar(1,nullptr);
    CreateGMMSettingsGUI();
    on_gmm_settings_trackbar(1,nullptr);
    
    while(1)
    {
        ApplySettings();
        ApplyGMMSettings();
        run();
        imshow(mainMatTitle, gmmMat);
        usleep(10000);
#ifdef SAVE_TO_FILE
        string fileName(string(ROOT_PATH) + this_descriptor
#ifdef SHOW_CLUSTERS
                        + "_clusters" + (states?"_states":"")
#endif
                        + ".png");
        imwrite(fileName, gmmMat);
#endif
        
        while(waitKey(50) != ' ')
        {
            if(waitKey(50) == 'p')
            {
                states = !states;
                break;
            }
        }
        
    }
    return 0;
}
