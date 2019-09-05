//
//  test_generator.hpp
//  ghmmtest
//
//  Created by Matthew Fonken on 9/4/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef test_generator_hpp
#define test_generator_hpp

#include <stdio.h>
#include <vector>
#include <random>
#include "matvec.h"

using namespace std;


class Set
{
    vector<pair<double, double>> V;
    int last_index = 0;
    int first_index = 0;
    
    default_random_engine generator;
    
public:
    void Generate( int, gaussian1d_t, gaussian1d_t );
    void Generate( gaussian1d_t, gaussian1d_t );
    void Push( double, double );
    pair<double, double> Pull( void );
    pair<double, double> Pop( void );
    void Reset( void );
};

#endif /* test_generator_hpp */
