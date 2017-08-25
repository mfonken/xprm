//
//  rho.c
//  tau+
//
//  Created by Matthew Fonken on 8/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

/* Own Include */
#include "rho.h"

ring_buffer_t peak_buffer;
prediction_pair_t last_locations;
prediction_pair_t last_densities;

void performTauA( cimage_t * img, uint16_t width, uint16_t height, prediction_pair_t * result )
{
    prediction_pair_t rho_result;
    performRho(img, width, height, &rho_result, &last_densities );
    performSigma( &rho_result, &last_locations, result);
    last_locations = *result;
}

void performRho( cimage_t * img, uint16_t width, uint16_t height, prediction_pair_t * location_predictions, prediction_pair_t * density_prediction )
{
    density_map_pair_t      density_map_pair;
    peak_list_pair_t        peak_list_pair;
    probability_list_pair_t probability_list_pair;
    
    generateDensityMap(     img, width, height,     &density_map_pair );
    getPeakListPair(        &density_map_pair,      &peak_list_pair );
    getProbabilityListPair( &peak_list_pair,        &probability_list_pair );
    getPredictions(         &probability_list_pair, &peak_list_pair, location_predictions, density_prediction);
}

void generateDensityMap( cimage_t * img, uint16_t width, uint16_t height, density_map_pair_t * density_maps )
{
    image_dimension_base_t p = 0;
    uint16_t row_sum;
    for( uint16_t i = 0; i < height; i++ )
    {
        row_sum = 0;
        for( uint16_t j = 0; j < width; j++ )
        {
            p = *img[i][j];
            row_sum += p;
            density_maps->y.map[j] += p;
        }
        density_maps->x.map[i] += row_sum;
    }
    density_maps->x.length = height;
    density_maps->y.length = width;
}

void getPeakListPair( density_map_pair_t * density_pair, peak_list_pair_t * peaks )
{
    getPeakList( &density_pair->x, &peaks->x );
    getPeakList( &density_pair->y, &peaks->y );
}

void getPeakList( density_map_t * density_map, peak_list_t * peaks )
{
    uint8_t peak_index = 0;
    uint16_t l = density_map->length, a,b,c;
    ringBufferAdd( &peak_buffer, density_map->map[0] );
    ringBufferAdd( &peak_buffer, density_map->map[1] );
    for( int i = 2; i < l; i++)
    {
        c = density_map->map[i];
        uint8_t last_index = ringBufferAdd( &peak_buffer, c );
        a = ringBufferRead( &peak_buffer, last_index-2 );
        b = ringBufferRead( &peak_buffer, last_index-1 );
        if( a < b && b < c )
        {
            peaks->locations[peak_index] = i - 1;
            peaks->peaks[peak_index++] = b;
        }
    }
    peaks->length = peak_index;
}

void getProbabilityListPair( peak_list_pair_t * peak_list_pair, probability_list_pair_t * probability_pair )
{
    getProbabilityList( &peak_list_pair->x, &probability_pair->x, &last_locations.x, &last_densities.x );
    getProbabilityList( &peak_list_pair->y, &probability_pair->y, &last_locations.y, &last_densities.y );
}

void getProbabilityList( peak_list_t * peaks, probability_list_t * probability, prediction_t* last_locations, prediction_t * last_densities )
{
    uint16_t length = peaks->length;
    double primary_location_probability, secondary_location_probability, primary_height_probability, secondary_height_probability;
    probability->length = length;
    for( uint16_t i = 0; i < length; i++ )
    {
        primary_location_probability    = 1 - ABS( 1 - ( last_locations->primary   / peaks->locations[i] ) );
        primary_height_probability      = 1 - ABS( 1 - ( last_densities->primary   / peaks->peaks[i] ) );
        
        secondary_location_probability  = 1 - ABS( 1 - ( last_locations->secondary / peaks->locations[i] ) );
        secondary_height_probability    = 1 - ABS( 1 - ( last_densities->secondary / peaks->peaks[i] ) );
        
        probability->primary_list[i]    = primary_location_probability   + DENSITY_BIAS * ( primary_height_probability   - primary_location_probability   );
        probability->secondary_list[i]  = secondary_location_probability + DENSITY_BIAS * ( secondary_height_probability - secondary_location_probability );
    }
}

void getPredictions( probability_list_pair_t * probability, peak_list_pair_t * peak_list_pair, prediction_pair_t * location_predictions, prediction_pair_t * density_predictions )
{
    getPredictionPair( &probability->x, &peak_list_pair->x, &location_predictions->x, &density_predictions->x );
    getPredictionPair( &probability->y, &peak_list_pair->y, &location_predictions->y, &density_predictions->y );
}

void getPredictionPair( probability_list_t * probability, peak_list_t * peak_list, prediction_t * location_predictions, prediction_t * density_prediction )
{
    uint16_t max_primary, max_secondary, current;
    uint16_t index_primary = 0, index_secondary = 0;
    max_primary = probability->primary_list[0];
    max_secondary = probability->secondary_list[0];
    for( uint16_t i = 1; i < probability->length; i++ )
    {
        current = probability->primary_list[i];
        if( current > max_primary )
        {
            max_primary = current;
            index_primary = i;
        }
        
        current = probability->secondary_list[i];
        if( current > max_secondary )
        {
            max_secondary = current;
            index_secondary = i;
        }
    }

    location_predictions->primary   = max_primary;
    location_predictions->secondary = max_secondary;
    density_prediction->primary     = peak_list->peaks[index_primary];
    density_prediction->secondary   = peak_list->peaks[index_secondary];
}
