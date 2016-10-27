//
//  spatial.c
//  
//
//  Created by Matthew Fonken on 10/8/16.
//
//

#include "spatial.h"
#include <stdio.h>
#include <iostream>

#define OUTPUT

using namespace std;

int main(int argc, char *args[])
{
    int initial_value   = 100;
    int new_value       = 100;
    double rate            = 1;
    double delta_time      = 1;
    
    if(argc >= 2)
    {
        initial_value = atoi(args[1]);
        if(argc >= 3)
        {
            new_value = atoi(args[2]);
            if(argc >= 4)
            {hing
                rate = atof(args[3]);
                if(argc >= 5)
                {
                    delta_time = atof(args[4]);
                }
            }
        }
    }
#ifdef OUTPUT
    cout << "Initializing Kalman:" << hex << &rot_f << " with inital value:" << initial_value << endl;
#endif
    initKalman(&rot_f[0], initial_value);

#ifdef OUTPUT
    cout << "Updating Kalman:" << hex << &rot_f << " with new value:" << dec << new_value << ", rate:" << rate << ", and ∆t:" << delta_time << endl;
#endif
    updateKalman(&rot_f[0], new_value, rate, delta_time);
    
    cout << "Updated Kalman is " << rot_f[0].value << "\n";
    
    cout << "I2C Read is " << (int)i2cRead(0x00, 0x00) << endl;
    return 0;
}
