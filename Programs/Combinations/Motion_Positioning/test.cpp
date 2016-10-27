//
//  test.cpp
//  
//
//  Created by Matthew Fonken on 10/15/16.
//
//

#include "spatial.h"
#include "pd.h"

#include <stdio.h>
#include <iostream>

#define OUTPUT

using namespace std;

int main(int argc, char *args[])
{
    double current_variance = 100;
    enum motion_pattern current_pattern = LINE;
    
    struct cartesian3 coords[2];
    coords[0].x = 0;
    coords[0].y = 1;
    coords[0].z = 2;
    coords[1].x = 3;
    coords[1].y = 4;
    coords[1].y = 5;
    
    struct cartesian3 new_coord;
    new_coord.x = 6;
    new_coord.y = 7;
    new_coord.y = 8;
    
    motionEvent( coords[1], coords, 2, 1, &current_pattern, &current_variance );
    cout << current_variance << endl;
    motionEvent( new_coord, coords, 2, 1, &current_pattern, &current_variance );
    cout << current_variance << endl;
}
