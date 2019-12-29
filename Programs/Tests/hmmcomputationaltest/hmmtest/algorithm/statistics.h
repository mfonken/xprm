//
//  statistics.h
//  combine
//
//  Created by Matthew Fonken on 8/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef statistics_h
#define statistics_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "rho_global.h"

typedef struct
{
    floating_t value;
    int count;
} cumulative_average_t;

void GenerateCumulativeMomentsStatistics( floating_t, floating_t, floating_t *, floating_t *, floating_t * );
void GenerateCumulativeAverageStatistics( floating_t, floating_t *, int * );
void CumulateAverageStatistics( floating_t, cumulative_average_t * );
    
#ifdef __cplusplus
}
#endif

#endif /* statistics_h */
