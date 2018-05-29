
//
//  general.hpp
//  redistributiontest
//
//  Created by Matthew Fonken on 5/28/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef general_h
#define general_h

typedef struct
{
    int Bx, Cx, By, Cy;
} centroids_t;

typedef struct
{
    int area[9];
} area_list_t;

typedef struct
{
    int
    background[4][4],
    current[4][4],
    factor[4][4],
    length[4];
} density_redistribution_lookup_config_t;

typedef struct
{
    const density_redistribution_lookup_config_t config[4];
    int area[9];
} density_redistribution_lookup_t;

#endif /* general_h */
