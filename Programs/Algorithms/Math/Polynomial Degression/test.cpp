//
//  test.cpp
//  
//
//  Created by Matthew Fonken on 10/15/16.
//
//

#include "pd.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

#define OUTPUT
#define CLOCK

const char* INPUT_FILENAME  = "input.txt";
const char* OUTPUT_FILENAME = "output.txt";

using namespace std;

int main(int argc, char *args[])
{

    ifstream file;
    ofstream outfile;
    size_t size = 0;
    file.open( INPUT_FILENAME, ios::in | ios::binary | ios::ate );
    outfile.open( OUTPUT_FILENAME, ios::out | ios::binary | ios::ate );
    char* data = 0;
    
    file.seekg(0);
    int l = 0;
    double t;
    while(file >> t) l++;
    int n = l / 3;
    printf("Found %d sets of coordinates in input file.\n", n);
    file.clear();
    file.seekg(0, ios::beg);

    double coords_temp[n][3];
    double next;
    
    for(int i = 0; i < n; i++)
    {
        outfile << "( ";
        for(int j = 0; j < 3; j++)
        {
            double next;
            file >> next;
            coords_temp[i][j] = next;
            outfile << next;
            if(j < 2) outfile << ", ";
//            printf("%.4f ", next);
        }
        outfile << " )\n";
//        printf("\n");
    }
    
    cartesian3 coords[n];
    for(int i = 0; i < n; i++)
    {
        coords[i].x = coords_temp[i][0];
        coords[i].y = coords_temp[i][1];
        coords[i].z = coords_temp[i][2];
    }
    
    cartesian3 M, H;
    double reg;
    printf("Performing regression.\n");
    
#ifdef CLOCK
    clock_t t1, t2;
    double diff;
    t1 = clock();
#endif
    reg = linearRegression3d( coords, n, &M, &H );
#ifdef CLOCK
    t2 = clock();
    diff = ((double)(t2 - t1) / 1000000.0F ) * 1000;
    printf("Processing took %f milliseconds", diff);
#endif
    printf(" Results:\n");
    printf("  Regression - %f\n", reg);
    printf("           M - (%.3f, %.3f, %.3f)\n", M.x, M.y, M.z);
    printf("           H - (%.3f, %.3f, %.3f)\n", H.x, H.y, H.z);
    
    outfile << "( " << M.x << ", " << M.y << ", " << M.z << " )\n";
    outfile << "( " << H.x << ", " << H.y << ", " << H.z << " )\n";
    
    file.close();
    outfile.close();
}
