//
//  global_types.h
//  tau+
//
//  Created by Matthew Fonken on 8/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef global_types_h
#define global_types_h

#ifdef __cplusplus
extern "C" {
#endif

#include "test_setup.h"
#include <stdint.h>
    
    typedef unsigned char   pixel_base_t;
    typedef struct
    {
        int width, height;
        pixel_base_t * pixels;
    } cimage_t;

#ifdef __cplusplus
}
#endif
    
#endif /* global_types_h */
