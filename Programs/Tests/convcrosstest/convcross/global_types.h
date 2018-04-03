//
//  global_types.h
//  convcross
//
//  Created by Matthew Fonken on 10/5/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef global_types_h
#define global_types_h

typedef double FLOAT;
typedef int    INT;

typedef struct
{
    FLOAT   *array;
    INT     length;
    FLOAT    sigma;
} gaussian_t;


typedef INT         image_dimension_base_t;
typedef INT         pixel_base_t;
typedef pixel_base_t**  cimage_t;

typedef struct
{
    FLOAT*          map;
    FLOAT*          vel;
    FLOAT*          acc;
    FLOAT*          jrk;
    FLOAT*          jnc;
    INT        length;
}density_map_t;

typedef struct
{
    density_map_t   x;
    density_map_t   y;
} density_map_pair_t;


#endif /* global_types_h */
