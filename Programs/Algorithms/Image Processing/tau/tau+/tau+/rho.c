//
//  rho.c
//  tau+
//
//  Created by Matthew Fonken on 8/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

/* Own Include */
#include "rho.h"
#include <stdlib.h>
#include <stdio.h>

ring_buffer_t peak_buffer;
prediction_pair_t last_locations, last_densities;
uint16_t image_width, image_height;

void initRho( prediction_pair_t * locations, prediction_pair_t * densities, uint16_t img_width, uint16_t img_height )
{
    printf("Init-ing rho: (%d, %d)\n", img_width, img_height);
    last_locations = *locations;
    last_densities = *densities;
    image_width = img_width;
    image_height = img_height;
}

void performRho( pixel_base_t ** img, prediction_pair_t * location_predictions, prediction_pair_t * density_prediction )
{
    density_map_pair_t*     density_map_pair = (density_map_pair_t *)malloc(sizeof(density_map_pair_t));
    density_map_pair->x.map = malloc (image_width * sizeof (uint16_t));
    density_map_pair->y.map = malloc (image_height * sizeof (uint16_t));
    peak_list_pair_t        peak_list_pair;
    probability_list_pair_t probability_list_pair;
    
    generateDensityMap(          img,                    density_map_pair );
    generatePeakListPair(        density_map_pair,       &peak_list_pair );
    generateProbabilityListPair( &peak_list_pair,        &probability_list_pair );
    generatePredictions(         &probability_list_pair, &peak_list_pair, location_predictions, density_prediction);
}

void generateDensityMap( pixel_base_t ** img, density_map_pair_t * density_maps )
{
    image_dimension_base_t p = 0;
    uint16_t row_sum;
    memset(density_maps->x.map, 0, sizeof(uint16_t)*density_maps->x.length);
    memset(density_maps->y.map, 0, sizeof(uint16_t)*density_maps->y.length);
    for( uint16_t i = 0; i < image_height; i++ )
    {
        row_sum = 0;
        for( uint16_t j = 0; j < image_width; j++ )
        {
            p = img[i][j];
            row_sum += p;
            density_maps->y.map[j] += p;
        }
        density_maps->x.map[i] += row_sum;
    }
    density_maps->x.length = image_height;
    density_maps->y.length = image_width;
}

void generatePeakListPair( density_map_pair_t * density_pair, peak_list_pair_t * peaks )
{
    generatePeakList( &density_pair->x, &peaks->x );
    generatePeakList( &density_pair->y, &peaks->y );
}

void generatePeakList( density_map_t * density_map, peak_list_t * peaks )
{
    printf("peaks %d\n", peaks->length);
    
    uint8_t peak_index = 0;
    uint16_t l = density_map->length, a,b,c,d,e;
    a = density_map->map[0];
    b = density_map->map[1];
    c = density_map->map[2];
    d = density_map->map[3];
    for( int i = 4; i < l; i++)
    {
        e = density_map->map[i];
        if( b > a && c >= b && c >= d && d >= e )
        {
            peaks->locations[peak_index] = i - 1;
            peaks->peaks[peak_index] = c;
            peak_index++;
        }
        a = b;
        b = c;
        c = d;
        d = e;
    }
    peaks->length = peak_index;
}

void generateProbabilityListPair( peak_list_pair_t * peak_list_pair, probability_list_pair_t * probability_pair )
{
    generateProbabilityList( &peak_list_pair->x, &probability_pair->x, &last_locations.x, &last_densities.x );
    generateProbabilityList( &peak_list_pair->y, &probability_pair->y, &last_locations.y, &last_densities.y );
}

void generateProbabilityList( peak_list_t * peaks, probability_list_t * probability, prediction_t* last_locations, prediction_t * last_densities )
{
    uint16_t length = peaks->length;
    FLOAT primary_location_probability, secondary_location_probability, primary_height_probability, secondary_height_probability;
    probability->length = length;
    for( uint16_t i = 0; i < length; i++ )
    {
        primary_location_probability    = 1 - fabs( (FLOAT)last_locations->primary   - (FLOAT)peaks->locations[i] ) / image_width;
        primary_height_probability      = 1 - fabs( (FLOAT)last_densities->primary   - (FLOAT)peaks->peaks[i]     ) / image_height;
        
        secondary_location_probability  = 1 - fabs( (FLOAT)last_locations->secondary - (FLOAT)peaks->locations[i] ) / image_height;
        secondary_height_probability    = 1 - fabs( (FLOAT)last_densities->secondary - (FLOAT)peaks->peaks[i]     ) / image_width;
        
        probability->primary[i]    = ( LOCATION_BIAS * primary_location_probability )   + ( DENSITY_BIAS * primary_height_probability );
        probability->secondary[i]  = ( LOCATION_BIAS * secondary_location_probability ) + ( DENSITY_BIAS * secondary_height_probability );
    }
}

void generatePredictions( probability_list_pair_t * probability, peak_list_pair_t * peak_list_pair, prediction_pair_t * location_predictions, prediction_pair_t * density_predictions )
{
    generatePredictionPair( &probability->x, &peak_list_pair->x, &location_predictions->x, &density_predictions->x );
    generatePredictionPair( &probability->y, &peak_list_pair->y, &location_predictions->y, &density_predictions->y );
}

void generatePredictionPair( probability_list_t * probability, peak_list_t * peak_list, prediction_t * location_predictions, prediction_t * density_prediction )
{
    FLOAT max_primary, max_secondary, current;
    uint16_t index_primary = 0, index_secondary = 0;
    max_primary = probability->primary[0];
    max_secondary = probability->secondary[0];
    
    for( uint16_t i = 1; i < probability->length; i++ )
    {
        current = probability->primary[i];
        if( current > max_primary )
        {
            max_primary = current;
            index_primary = i;
        }
        current = probability->secondary[i];
        if( current > max_secondary )
        {
            max_secondary = current;
            index_secondary = i;
        }
    }
    location_predictions->primary   = peak_list->locations[index_primary];
    location_predictions->secondary = peak_list->locations[index_secondary];
    density_prediction->primary     = peak_list->peaks[index_primary];
    density_prediction->secondary   = peak_list->peaks[index_secondary];
}
