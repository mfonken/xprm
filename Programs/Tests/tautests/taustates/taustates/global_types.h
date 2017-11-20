//
//  global_types.h
//  taustates
//
//  Created by Matthew Fonken on 10/17/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef global_types_h
#define global_types_h

#include <stdbool.h>

typedef int16_t image_dimension_base_t;

/* Tau states */
typedef enum
{
    UNKNOWN_STATE = 0,
    STABLE_NONE,
    UNSTABLE_NONE,
    STABLE_SINGLE,
    UNSTABLE_SINGLE,
    STABLE_DOUBLE,
    UNSTABLE_DOUBLE,
    STABLE_MANY,
    UNSTABLE_MANY,
//    COLLAPSING,
    
    NUM_STATES
} state_t;

static inline int stateToSelection(state_t s) {return ((int)((s+1)/2) - 1);};

static inline const char *stateString(state_t s)
{
    static const char *strings[] = { "UN", "S0", "U0", "S1", "U1", "S2", "U2", "SM", "UM", "CO" };
    return strings[s];
}

typedef enum
{
    SIMILAR = 0,
    OPPOSITE
} selection_pair_t;

typedef struct
{
    image_dimension_base_t  primary;
    double                  primary_probability;
    image_dimension_base_t  secondary;
    double                  secondary_probability;
    double                  alternate_probability;
} prediction_t;

typedef struct
{
    prediction_t x;
    prediction_t y;
    selection_pair_t        selection_pair;
} prediction_pair_t;

/* Markov state tree with bayesian base */
typedef struct
{
    double  map[NUM_STATES][NUM_STATES];
    int     length;
    bool    scheduled_normalize;
} state_bayesian_map_t;

/* Tau filter types specifically for kalman matrix structure */
typedef enum
{
    NO_FILTER = 0,
    SOFT_FILTER,
    HARSH_FILTER,
    CHAOS_FILTER
} filter_t;

/* Stability tracking for selec tions */
typedef struct
{
    double primary;
    double secondary;
    double alternate;
    double overall;
} stability_t;

/* System self-diagnostic state control type */
typedef struct
{
    state_t     state;
    state_t     prev;
    state_t     next;
    int         selection_index;
    stability_t stability;
    filter_t    filter;
    state_bayesian_map_t probabilities;
} system_t;

#endif /* global_types_h */
