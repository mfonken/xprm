//
//  tau_test.hpp
//  tau+
//
//  Created by Matthew Fonken on 8/27/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef tau_test_hpp
#define tau_test_hpp

#include "global_types.h"

#include <stdio.h>
#include <stdint.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;

void drawTau(Mat M, density_map_pair_t * m, peak_list_pair_t * p, probability_list_pair_t * r, prediction_pair_t * e);

void print2DArray( pixel_base_t ** i, int w, int h );
void initImg( pixel_base_t *** i, int w, int h );
void drawDensityMap(density_map_pair_t * m);
void drawDensityMapWithPeaks(density_map_pair_t * m, peak_list_pair_t * p);
void drawDensityMapWithProbabilities(density_map_pair_t * m, peak_list_pair_t * p, probability_list_pair_t * r);
void drawDensityMapWithPredictions(density_map_pair_t * m, peak_list_pair_t * p, probability_list_pair_t * r, prediction_pair_t * e);


#endif /* tau_test_hpp */
