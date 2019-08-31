//
//  main.cpp
//  OV9712_autotest
//
//  Created by Matthew Fonken on 7/7/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "OV9712_calc.h"

#define MIN_PERCENT_ACTIVE  0.00
#define MAX_PERCENT_ACTIVE  1.0
#define STEP_PERCENT_ACTIVE 0.02

#define MIN_FRAME_RATE      1.0
#define MAX_FRAME_RATE      30
#define STEP_FRAME_RATE     1.0

using namespace std;

template <typename T>
static string to_stringn(const T a_value, const int n = 6)
{
    ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

void GenerateFields(void)
{
    ofstream file1("/Users/matthew/Desktop/OV9712_optimization_table1_" + to_string(_subSample) + ".csv");
    ofstream file2("/Users/matthew/Desktop/OV9712_optimization_table2_" + to_string(_subSample) + ".csv");
    ofstream file3("/Users/matthew/Desktop/OV9712_optimization_table3_" + to_string(_subSample) + ".csv");
    for(double frame_rate = MIN_FRAME_RATE; frame_rate <= MAX_FRAME_RATE; frame_rate += STEP_FRAME_RATE )
    {
        file1 << "," << to_stringn(frame_rate, 1);
        file2 << "," << to_stringn(frame_rate, 1);
        file3 << "," << to_stringn(frame_rate, 1);
    }
    file1 << endl;
    file2 << endl;
    file3 << endl;
    
    for(double percent_active = MIN_PERCENT_ACTIVE; percent_active <= MAX_PERCENT_ACTIVE; percent_active += STEP_PERCENT_ACTIVE )
    {
        SetPercentActive(percent_active);
        file1 << to_stringn(percent_active, 2);
        file2 << to_stringn(percent_active, 2);
        file3 << to_stringn(percent_active, 2);
        for(double frame_rate = MIN_FRAME_RATE; frame_rate <= MAX_FRAME_RATE; frame_rate += STEP_FRAME_RATE )
        {
            SetFrameRate(frame_rate);
            
            file1 << "," << to_stringn(PERCENT_REAL_TIME, 2);
            file2 << "," << to_stringn(MID_FRAME_SLACK, 5);
            file3 << "," << to_stringn(ACTUAL_FRAME_RATE, 1);
        }
        file1 << endl;
        file2 << endl;
        file3 << endl;
    }
    
    file1.close();
    file2.close();
    file3.close();
}


int main(int argc, const char * argv[])
{
//    GenerateFields();
    
    printf("  Capture: %d\n", CAPTURE_BUFFER_LENGTH);
    printf("   Thresh: %d\n", THRESH_BUFFER_LENGTH * 2);
    printf("Density X: %d\n", DENSITY_MAP_X_LENGTH * 2);
    printf("Density Y: %d\n", DENSITY_MAP_Y_LENGTH * 2);
    
    printf("  PLL_PRE: %d\n", PLL_PRE_DIVIDER);
    printf("  PLL_MUL: %d\n", PLL_MULTIPLIER);
    printf("  PLL_DIV: %d\n", PLL_DIVIDER);
    printf("  CLK_DIV: %d\n", CLK_DIVIDER);

#ifdef REG5C_V
    printf("REG5C_V: 0x%02x\n", REG5C_V);
    printf("REG5D_V: 0x%02x\n", REG5D_V);
    printf("REG03_V: 0x%02x\n", REG03_V);
    printf("REG32_V: 0x%02x\n", REG32_V);
    printf("HSTART_V: 0x%02x\n", HSTART_V);
    printf("VSTART_V: 0x%02x\n", VSTART_V);
    printf("AHSIZE_V: 0x%02x\n", AHSIZE_V);
    printf("AVSIZE_V: 0x%02x\n", AVSIZE_V);
    
    printf("DSP_CTRL_1_V: 0x%02x\n", DSP_CTRL_1_V);
    
    printf("FRAME_HSTART: %d\n", FRAME_HSTART);
    printf("FRAME_VSTART: %d\n", FRAME_VSTART);
    printf("CAMERA_SENSOR_WIDTH: %d\n", CAMERA_SENSOR_WIDTH);
#endif
    
    printf("   SYSCLK: %8d\n", SYSCLK(_frameRate));
    printf("     XCLK: %8d\n", XCLK(_frameRate));
    printf("     CLK2: %8d\n", CLK2(_frameRate));
    
    return 0;
}
