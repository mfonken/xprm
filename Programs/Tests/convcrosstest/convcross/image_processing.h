//
//  image_processing.h
//  convcross
//
//  Created by Matthew Fonken on 10/5/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef image_processing_h
#define image_processing_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include "global_types.h"
    
#define DEFAULT_SIGMA 3.0
    
    
void generateDensityMap( pixel_base_t ** img, int image_width, int image_height, density_map_pair_t * density_maps );
    
#ifdef __cplusplus
}
#endif
    
#endif /* image_processing_h */
