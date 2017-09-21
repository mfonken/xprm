//
//  rho.h
//  tau+
//
//  Created by Matthew Fonken on 8/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifdef __cplusplus
extern "C" {
#endif

#ifndef rho_h
#define rho_h

#include <stdio.h>

/* Global Included */
#include "global_types.h"
#include "ring_buffer.h"
#include "sigma.h"

extern ring_buffer_t peak_buffer;

void initRho( prediction_pair_t * locations, prediction_pair_t * densities, uint16_t img_width, uint16_t img_height );
void performRho( pixel_base_t ** img, prediction_pair_t * location_predictions, prediction_pair_t * density_prediction );
void generateDensityMap( pixel_base_t ** img, density_map_pair_t * density_maps );
void generatePeakListPair( density_map_pair_t * map, peak_list_pair_t * peaks );
void generatePeakList( density_map_t * map, peak_list_t * peaks );
void generateProbabilityListPair( peak_list_pair_t * peak_list_pair, probability_list_pair_t * probability_pair );
void generateProbabilityList( peak_list_t * peaks, probability_list_t * probability, prediction_t* last_locations, prediction_t* last_densities );
void generatePredictions( probability_list_pair_t * probability, peak_list_pair_t * peak_list_pair, prediction_pair_t * location_predictions, prediction_pair_t * density_predictions );
void generatePredictionPair( probability_list_t * probability, peak_list_t * peak_list, prediction_t * location_predictions, prediction_t * density_prediction );

#endif /* rho_h */
    
#ifdef __cplusplus
}
#endif
