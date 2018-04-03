//
//  rho_wrapper.hpp
//  tau+
//
//  Created by Matthew Fonken on 3/26/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef rho_wrapper_hpp
#define rho_wrapper_hpp

#include <stdio.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "test_setup.h"

/* Global Included */
#include "global_types.h"
#include "test_setup.h"
#include "rho_kalman.h"

#include "tau_structures.hpp"
//#include "gaussian.hpp"
//#include "kalman.hpp"

#include "utility_functions.hpp"
#include "rho_utility.h"

class Rho
{
public:
    int                 width,
                        height;
    
    cimage_t            image;
    cv::Mat             frame;
    rho_c_utility       utility;
    pthread_mutex_t     c_mutex;
    pthread_mutex_t     frame_mutex;
    pthread_mutex_t     density_map_pair_mutex;
    
    Rho( int, int );
    void perform( cimage_t *, PredictionPair * );
    void perform( cv::Mat, PredictionPair * );    
};

#endif /* rho_wrapper_hpp */
