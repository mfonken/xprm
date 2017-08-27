//
//  tau.c
//  tau+
//
//  Created by Matthew Fonken on 8/25/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "tau.h"
#include "rho.h"

prediction_pair_t locations;
prediction_pair_t densities;

void initTauA( uint16_t width, uint16_t height )
{
    locations.x.primary     = DEFAULT_X_PRIMARY_LOCATION;
    locations.x.secondary   = DEFAULT_X_SECONDARY_LOCATION;
    locations.y.primary     = DEFAULT_Y_PRIMARY_LOCATION;
    locations.y.secondary   = DEFAULT_Y_SECONDARY_LOCATION;
    
    densities.x.primary     = DEFAULT_X_PRIMARY_DENSITY;
    densities.x.secondary   = DEFAULT_X_SECONDARY_DENSITY;
    densities.y.primary     = DEFAULT_Y_PRIMARY_DENSITY;
    densities.y.secondary   = DEFAULT_Y_SECONDARY_DENSITY;

    initRho( &locations, &densities, width, height );
}

void performTauA( pixel_base_t ** img, prediction_pair_t * result )
{
    prediction_pair_t rho_result;
    performRho(img, &rho_result, &densities );
    performSigma( &rho_result, &densities, result);
    densities = *result;
}

void getDensityMap( pixel_base_t ** img, density_map_pair_t * result )
{
    generateDensityMap(img, result);
}

void getPeakList( density_map_pair_t * map, peak_list_pair_t * peaks )
{
    generatePeakListPair(map, peaks);
}

void getProbabilityList( peak_list_pair_t * peaks, probability_list_pair_t * prob )
{
    generateProbabilityListPair(peaks, prob);
}
void getPredictions( peak_list_pair_t * peaks, probability_list_pair_t * prob, prediction_pair_t * pred )
{
    generatePredictions( prob, peaks, pred, &densities );
}

void initDensityMapPair( density_map_pair_t * m, int w, int h )
{
    (*m).x.map = (uint16_t*)malloc(sizeof(uint16_t)*h);
    (*m).y.map = (uint16_t*)malloc(sizeof(uint16_t)*w);
    (*m).x.length = h;
    (*m).y.length = w;
}
void initPeaksListPair( peak_list_pair_t * p, int w, int h )
{
    (*p).x.locations = (uint16_t*)malloc(sizeof(uint16_t)*h/2);
    (*p).x.peaks = (uint16_t*)malloc(sizeof(uint16_t)*h/2);
    (*p).y.locations = (uint16_t*)malloc(sizeof(uint16_t)*w/2);
    (*p).y.peaks = (uint16_t*)malloc(sizeof(uint16_t)*w/2);
}
void initProbabilityListPair( probability_list_pair_t * r, int w, int h )
{
    (*r).x.primary = (FLOAT*)malloc(sizeof(FLOAT)*h/2);
    (*r).x.secondary = (FLOAT*)malloc(sizeof(FLOAT)*h/2);
    (*r).y.primary = (FLOAT*)malloc(sizeof(FLOAT)*w/2);
    (*r).y.secondary = (FLOAT*)malloc(sizeof(FLOAT)*w/2);
}

