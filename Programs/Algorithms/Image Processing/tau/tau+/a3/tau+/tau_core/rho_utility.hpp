//
//  rho_utility.hpp
//  tau+
//
//  Created by Matthew Fonken on 2/7/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef rho_utility_hpp
#define rho_utility_hpp

#include <stdio.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "test_setup.h"

/* Global Included */
#include "global_types.h"
#include "tau_structures.hpp"
#include "gaussian.hpp"
#include "kalman.hpp"

#include "utility_functions.hpp"

class Rho
{
public:
    int                width;
    int                height;
    cimage_t                image;
    cv::Mat                 frame;
    DensityMapPair          density_map_pair;
    PeakListPair            peak_list_pair;
    Gaussian                gaussian;
    
    Rho( int, int );
    void perform( cimage_t *, PredictionPair * );
    void perform( cv::Mat, PredictionPair * );
    void generateDensityMap();
    void generateDensityMapFromCImage();
    void getDensityMaxPair();
    void getDensityMax( DensityMap * );
    void updateDensityKalmanPair();
    void updateDensityKalman( DensityMap * );
    void filterDensityPair();
    void filterDensity( DensityMap * );
    void analyzeDensityPair();
    void analyzeDensity( DensityMap *, PeakList * );
    void selectPeakListPair( PredictionPair * );
    void selectPeakList( PeakList * p, Prediction * );
};

#endif /* rho_utility_hpp */
