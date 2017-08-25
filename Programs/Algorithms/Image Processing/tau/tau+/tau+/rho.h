//
//  rho.h
//  tau+
//
//  Created by Matthew Fonken on 8/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef rho_h
#define rho_h

#include <stdio.h>

/* Global Included */
#include "global_types.h"
#include "ring_buffer.h"
#include "sigma.h"

extern ring_buffer_t peak_buffer;

void performTauA( cimage_t * img, uint16_t width, uint16_t height, prediction_pair_t * result );
void performRho( cimage_t * img, uint16_t width, uint16_t height, prediction_pair_t * location_predictions, prediction_pair_t * density_prediction );

void generateDensityMap( cimage_t * img, uint16_t width, uint16_t height, density_map_pair_t * density_maps );
void getPeakListPair( density_map_pair_t * map, peak_list_pair_t * peaks );
void getPeakList( density_map_t * map, peak_list_t * peaks );
void getProbabilityListPair( peak_list_pair_t * peak_list_pair, probability_list_pair_t * probability_pair );
void getProbabilityList( peak_list_t * peaks, probability_list_t * probability, prediction_t* last_locations, prediction_t* last_densities );
void getPredictions( probability_list_pair_t * probability, peak_list_pair_t * peak_list_pair, prediction_pair_t * location_predictions, prediction_pair_t * density_predictions );
void getPredictionPair( probability_list_t * probability, peak_list_t * peak_list, prediction_t * location_predictions, prediction_t * density_prediction );

#endif /* rho_h */
