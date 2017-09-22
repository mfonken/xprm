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
uint16_t image_width, image_height;
density_map_pair_t      density_map_pair;
peak_list_pair_t        peak_list_pair;
probability_list_pair_t probability_list_pair;

void initRho( uint16_t img_width, uint16_t img_height )
{
    printf("Init-ing rho: (%d, %d)\n", img_width, img_height);
    image_width  = img_width;
    image_height = img_height;
    
    initDensityMapPair(     &density_map_pair, image_width, image_height);
    initPeaksListPair(      &peak_list_pair, image_width, image_height);
    initProbabilityListPair(&probability_list_pair, image_width, image_height);
}

void performRho( pixel_base_t ** img, prediction_pair_t * location_predictions, prediction_pair_t * density_predictions  )
{
    generateDensityMap(      img,     &density_map_pair );
    generatePeakListPair(             &density_map_pair,      &peak_list_pair );
    generateProbabilityListPair(      &peak_list_pair,        &probability_list_pair, location_predictions, density_predictions );
    generatePredictions(              &probability_list_pair, &peak_list_pair,        location_predictions, density_predictions );
}

void getRhoData( density_map_pair_t * d, peak_list_pair_t * p, probability_list_pair_t * r )
{
    *d = density_map_pair;
    *p = peak_list_pair;
    *r = probability_list_pair;
}

void generateDensityMap( pixel_base_t ** img, density_map_pair_t * density_maps )
{
    image_dimension_base_t p = 0;
    uint16_t row_sum;
    memset(density_maps->x.map, 0, sizeof(uint16_t) * density_maps->x.length);
    memset(density_maps->y.map, 0, sizeof(uint16_t) * density_maps->y.length);
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
    smooth1D(density_map_pair.x.map, density_map_pair.x.length);
    smooth1D(density_map_pair.y.map, density_map_pair.y.length);
}

void generatePeakListPair( density_map_pair_t * density_pair, peak_list_pair_t * peaks )
{
    generatePeakList( &density_pair->x, &peaks->x );
    generatePeakList( &density_pair->y, &peaks->y );
}

void generatePeakList( density_map_t * density_map, peak_list_t * peaks )
{
//    printf("peaks %d\n", peaks->length);
    
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

void generateProbabilityListPair( peak_list_pair_t * peak_list_pair, probability_list_pair_t * probability_pair, prediction_pair_t * last_locations, prediction_pair_t * last_densities )
{
    generateProbabilityList( &peak_list_pair->x, &probability_pair->x, &last_locations->x, &last_densities->x );
    generateProbabilityList( &peak_list_pair->y, &probability_pair->y, &last_locations->y, &last_densities->y );
}

void generateProbabilityList( peak_list_t * peaks, probability_list_t * probability, prediction_t* last_locations, prediction_t * last_densities )
{
    uint16_t length = peaks->length;
    FLOAT primary_location_probability, secondary_location_probability, primary_density_probability, secondary_density_probability;
    FLOAT llp = (FLOAT)last_locations->primary, lls = (FLOAT)last_locations->secondary;
    FLOAT ldp = (FLOAT)last_densities->primary, lds = (FLOAT)last_densities->secondary;
    probability->length = length;
//    printf("Probability length is %d\n", length);
    for( uint16_t i = 0; i < length; i++ )
    {
        primary_location_probability    = 1 - fabs( llp - (FLOAT)peaks->locations[i] ) / image_width;
        primary_density_probability      = 1 - fabs( ldp - (FLOAT)peaks->peaks[i]     ) / image_height;
        
        secondary_location_probability  = 1 - fabs( lls - (FLOAT)peaks->locations[i] ) / image_height;
        secondary_density_probability    = 1 - fabs( lds - (FLOAT)peaks->peaks[i]     ) / image_width;
        
        probability->primary[i]    = ( LOCATION_BIAS * primary_location_probability )   + ( DENSITY_BIAS * primary_density_probability );
        probability->secondary[i]  = ( LOCATION_BIAS * secondary_location_probability ) + ( DENSITY_BIAS * secondary_density_probability );
//        printf("\t Last[%d]: primary-location>%.2f & density>%.2f secondary-location>%.2f & density>%.2f\n", i, llp, ldp, lls, lds );
//        printf("  Primary[%d] location>%.2f & density>%.2f\n", i, primary_location_probability, primary_density_probability );
//        printf("Secondary[%d] location>%.2f & density>%.2f\n", i, secondary_location_probability, secondary_density_probability );
//        printf("Probability[%d] is pri>%.2f & sec>%.2f\n", i, probability->primary[i], probability->secondary[i]);
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
    max_primary   = probability->primary[0];
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
    
//    printf("Peak list length is %d(%d)\n", peak_list->length, probability->length);
//    printf("Indeces: pri>%d & sec>%d\n", index_primary, index_secondary);
    if(peak_list->length)
    {
        location_predictions->primary   = peak_list->locations[index_primary];
        location_predictions->secondary = peak_list->locations[index_secondary];
        density_prediction->primary     = peak_list->peaks[index_primary];
        density_prediction->secondary   = peak_list->peaks[index_secondary];
    }
}

void smooth1D( uint16_t * arr, uint16_t len )
{
    uint16_t a,b,c,d,e,f,g;
    a = arr[0];
    b = arr[1];
    c = arr[2];
    d = arr[3];
    e = arr[4];
    f = arr[5];
    for(uint16_t i = 6; i < len; i++)
    {
        g = arr[i];
        arr[i] = ( a + b + c + d + e + f + g ) / 7;
        a = b;
        b = c;
        c = d;
        d = e;
        e = f;
        f = g;
    }
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
