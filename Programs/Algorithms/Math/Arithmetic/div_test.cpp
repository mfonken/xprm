//
//  div_test.cpp
//  
//
//  Created by Matthew Fonken on 10/4/16.
//
//

#include <stdio.h>
#include <stdlib.h>     /* atoi */
#include <iostream>

using namespace std;

int main(int argc, char *args[])
{
    if(argc == 2)
    {
        int A = atoi(args[1]);
        cout << (((A * 0x0000AAAB) >> 16) >> 1) << endl;
    }
    return 0;
}