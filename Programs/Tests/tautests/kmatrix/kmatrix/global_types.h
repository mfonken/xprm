//
//  global_types.h
//  kmatrix
//
//  Created by Matthew Fonken on 10/9/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef global_types_h
#define global_types_h

#include "kalman.h"
#include <stdint.h>

#define MAX_KALMANS 3
#define MAX_PEAKS   3

/* Kalman Internal Lookup Pointer Type */
typedef uint8_t kmat_p;

typedef uint8_t lookup_t;

/* Kalman Internal Float Type */
typedef double kmat_f;

/* Kalman Probabilities Type */
typedef struct
{
    kmat_f  drop;
    kmat_f  prev_weight;
    kmat_f  curr_weight;
} kmat_probabilities_t;

/* Kalman Matrix Type */
typedef struct
{
    kalman_t    kalmans[MAX_KALMANS][MAX_PEAKS];
    lookup_t    lookup[MAX_KALMANS];            /**< Tracks Kalman resorting >*/
    kmat_p      selection[MAX_KALMANS];         /**< Tracks current peak id selection >*/
    double      value[MAX_KALMANS];             /**< Tracks current tracking value >*/
    double      density[MAX_KALMANS];           /**< Tracks current density value >*/
    uint8_t     pair[MAX_KALMANS];
    kmat_p      k_index;
    kmat_p      p_index;
} kmat_t;

/* Kalman Matrix Type */
typedef struct
{
    kmat_t  primary;
    kmat_t  secondary;
} kmat_ext_t;

/* Kalman Matrix Pair Type */
typedef struct
{
    kmat_ext_t  x;
    kmat_ext_t  y;
    kmat_probabilities_t      probabilities;
} kmat_pair_t;

/* Peak List Type */
typedef struct
{
    uint16_t    length;
    uint16_t*   map;
    uint16_t*   density;
    int*        direction;
}peak_list_t;

/* Peak List Pair Type */
typedef struct
{
    peak_list_t x;
    peak_list_t y;
} peak_list_pair_t;

typedef struct
{
    uint16_t  length;
    double  * values;
    lookup_t* lookup;
} probability_list_t;

typedef struct
{
    probability_list_t primary;
    probability_list_t secondary;
} probability_list_ext_t;

typedef struct
{
    probability_list_ext_t x;
    probability_list_ext_t y;
} probability_list_pair_t;

#endif /* global_types_h */
