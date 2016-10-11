//
//  spatial.c
//  
//
//  Created by Matthew Fonken on 10/8/16.
//
//

#include "spatial.h"
#include <stdio.h>

int main(int argc, char *args[])
{
    for(int i = 0; i < 3; i++)
    {
        pos[i] = 0;
        rot[i] = 0;
        kalmanInit(&pos_f[i]);
        kalmanInit(&rot_f[i]);
    }
    
    return 0;
}
