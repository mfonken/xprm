//
//  trig-lookup-generator.cpp
//  
//
//  Created by Matthew Fonken on 11/1/16.
//
//

#include <stdio.h>
#include <iostream>
#include <math.h>

#define STEPS       512
#define LINE_LENGTH 4
#define QUADRANTS   1

#define ATAN_MAX    M_PI * 5

//#define HEX

using namespace std;

double quadrant_f[4] = { 0, M_PI/2, -M_PI, - M_PI/2 };

void generateSinTable()
{
    cout << "/* Sine Lookup Table - " << STEPS << " values per " << QUADRANTS << " quadrant" << (QUADRANTS > 1 ? "s ":" ") << "(" << ( STEPS * QUADRANTS * 4)/1024 << "kb) */" << endl;
#ifndef HEX
    cout << "float";
#else
    cout << "int";
#endif
    cout << " sin_lookup_h[" << STEPS * QUADRANTS << "] =" << endl;
    cout << "{\n\t";
    double p;
    for(int q = 0; q < QUADRANTS; q++)
    {
        for(int d = 0; d < STEPS; d+=LINE_LENGTH)
        {
            for(int l = 0; l < LINE_LENGTH; l++)
            {
                p = ((double)(d+l)/STEPS)*M_PI/2 + quadrant_f[q];
                //cout << "p is " << p << endl;
                float r = sin(p);

#ifndef HEX
                if(r >= 0)
                    cout << " ";
                printf("%.10f, ", r);
#else
                int h = 0xfffffff * r;
                printf("0x%08x, ", h);
#endif
            }
            cout << endl << "\t";
        }
    }
    cout << "}" << endl;
}

void generateCosTable()
{}

void generateAtanTable()
{
    cout << "/* Atan Lookup Table - " << STEPS << " values per " << QUADRANTS << " quadrant" << (QUADRANTS > 1 ? "s ":" ") << "(" << ( STEPS * QUADRANTS * 4)/1024 << "kb) " << " - [0, " << ATAN_MAX << "rad] */" << endl;
#ifndef HEX
    cout << "float";
#else
    cout << "int";
#endif
    cout << " atan_lookup_h[" << STEPS * QUADRANTS << "] =" << endl;
    cout << "{\n\t";
    double p;
    for(int q = 0; q < QUADRANTS; q++)
    {
        for(int d = 0; d < STEPS; d+=LINE_LENGTH)
        {
            for(int l = 0; l < LINE_LENGTH; l++)
            {
                p = ((double)(d+l)/STEPS)*ATAN_MAX;
                float r = atan(p);

//                 cout << "r is " << r << endl;
#ifndef HEX
                if(r >= 0)
                    cout << " ";
                printf("%.10f, ", r);
#else
                int h = 0xfffffff * r;
                printf("0x%08x, ", h);
#endif
            }
            cout << endl << "\t";
        }
    }
    cout << "}" << endl;
}

void generateSinAtanTable()
{
    cout << "/* sin/cos(atan()) Lookup Table - " << STEPS << " values per " << QUADRANTS << " quadrant" << (QUADRANTS > 1 ? "s ":" ") << "(" << ( STEPS * QUADRANTS * 4 * 2)/1024 << "kb) " << " - [0, " << ATAN_MAX << "rad] */" << endl;
#ifndef HEX
    cout << "float";
#else
    cout << "int";
#endif
    cout << " sin_atan_lookup_h[" << STEPS * QUADRANTS << "][2] =" << endl;
    cout << "{\n\t";
    double p;
    for(int q = 0; q < QUADRANTS; q++)
    {
        for(int d = 0; d < STEPS; d+=LINE_LENGTH)
        {
            for(int l = 0; l < LINE_LENGTH; l++)
            {
                p = ((double)(d+l)/STEPS)*ATAN_MAX;
                float si = sin(atan(p));
                float co = cos(atan(p));
                
                //                 cout << "r is " << r << endl;
#ifndef HEX
                printf("{ %.10f, %.10f }, ", si, co);
#else
                int g = 0xfffffff * si;
                int h = 0xfffffff * co;
                printf("{ 0x%08x, 0x%08x }, ", g, h);
#endif
            }
            cout << endl << "\t";
        }
    }
    cout << "}" << endl;
}

int main(int argc, char *args[])
{
    char in;
    if(argc >= 2)
    {
        in = args[1][0];
    }
    switch(in)
    {
        case 's':
            generateSinTable();
            break;
        case 'c':
            generateCosTable();
            break;
        case 'a':
            generateAtanTable();
            break;
        case '1':
            generateSinAtanTable();
            break;
        default:
            cout << "Size of float is " << sizeof(float) << " bytes."<< endl;
            break;
    }
    
    return 0;
}
