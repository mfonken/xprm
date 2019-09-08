//
//  test_generator.cpp
//  ghmmtest
//
//  Created by Matthew Fonken on 9/4/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "test_generator.hpp"

void Set::Generate( int len, gaussian1d_t A )
{
    normal_distribution<double> distA( A.mean, A.std_dev);
    while( len-- > 0 )
        Push( distA(generator), 0. );
}
void Set::Generate( int len, gaussian1d_t A, gaussian1d_t B )
{
    normal_distribution<double> distA( A.mean, A.std_dev);
    normal_distribution<double> distB( B.mean, B.std_dev );
    while( len-- >= 0 )
        Push( distA(generator), distB(generator) );
}
void Set::Generate( gaussian1d_t A, gaussian1d_t B )
{
    normal_distribution<double> distA( A.mean, A.std_dev );
    normal_distribution<double> distB( B.mean, B.std_dev );
    Push( distA(generator), distB(generator) );
}
void Set::Push(double a, double b)
{
    last_index++;
    V.push_back( make_pair(a, b));
}
pair<double, double> Set::Pull(void)
{
    if( V.size() > first_index )
        first_index++;
    return V[first_index];
}
pair<double, double> Set::Pop(void)
{
    if( last_index > 0 )
        --last_index;
    return V[last_index];
}
void Set::Reset(void)
{
    V.clear();
    first_index = 0;
    last_index = 0;
}
int Set::Length(void)
{
    return (int)V.size()-1;
}
