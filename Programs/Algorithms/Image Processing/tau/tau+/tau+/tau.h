//
//  tau.h
//  tau+
//
//  Created by Matthew Fonken on 8/25/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifdef __cplusplus
extern "C" {
#endif

#ifndef tau_h
#define tau_h

#include <stdio.h>
#include "global_types.h"

#define DEFAULT_X_PRIMARY_LOCATION      50
#define DEFAULT_X_SECONDARY_LOCATION    150
#define DEFAULT_Y_PRIMARY_LOCATION      30
#define DEFAULT_Y_SECONDARY_LOCATION    110

#define DEFAULT_X_PRIMARY_DENSITY       10
#define DEFAULT_X_SECONDARY_DENSITY     20
#define DEFAULT_Y_PRIMARY_DENSITY       10
#define DEFAULT_Y_SECONDARY_DENSITY     20
    
void initTauA( uint16_t width, uint16_t height );
void performTauA( pixel_base_t ** img, prediction_pair_t * result );
void getDensityMap( pixel_base_t ** img, density_map_pair_t * result );
void getPeakList( density_map_pair_t * map, peak_list_pair_t * peaks );
void getProbabilityList( peak_list_pair_t * peaks, probability_list_pair_t * prob );
void getPredictions( peak_list_pair_t * peaks, probability_list_pair_t * prob, prediction_pair_t * pred );

void initDensityMapPair( density_map_pair_t * m, int w, int h );
void initPeaksListPair( peak_list_pair_t * p, int w, int h );
void initProbabilityListPair( probability_list_pair_t * r, int w, int h );
    
#endif /* tau_h */

#ifdef __cplusplus
}
#endif
