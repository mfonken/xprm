//
//  image_processing.c
//  convcross
//
//  Created by Matthew Fonken on 10/5/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "image_processing.h"
#include <string.h>

void generateDensityMap( pixel_base_t ** img, INT image_width, INT image_height, density_map_pair_t * density_maps )
{
    image_dimension_base_t p = 0;
    INT row_sum;
    memset(density_maps->x.map, 0, sizeof(FLOAT) * density_maps->x.length);
    memset(density_maps->y.map, 0, sizeof(FLOAT) * density_maps->y.length);
    for( INT i = 0; i < image_height; i++ )
    {
        row_sum = 0;
        for( INT j = 0; j < image_width; j++ )
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
