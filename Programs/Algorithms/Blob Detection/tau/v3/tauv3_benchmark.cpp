#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <string.h>
#include <sstream>

#define xkl 20
#define ykl 20

#define MAX_PEAKS 100

#define MIN_DENSITY 1500

#define DEBUG

double xk[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
double yk[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int xcl;
int ycl;


int xpn = 0;
int ypn = 0;

int xp[MAX_PEAKS], yp[MAX_PEAKS];


const char* INPUT_FILENAME = "images/shapes.bmp";
const char* OUTPUT_FILENAME = "images/output.bmp";

using namespace std;



void convolve(const double Signal[], size_t SignalLen,
              const double Kernel[], size_t KernelLen,
              double Result[])
{
    size_t n;
    int i = 0;
    for (n = 0; n < SignalLen + KernelLen - 1; n++)
    {
        size_t kmin, kmax, k;
        
        Result[n] = 0;
        
        kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
        kmax = (n < SignalLen - 1) ? n : SignalLen - 1;
        
        
        for (k = kmin; k <= kmax; k++)
        {
            i++;
            Result[n] += Signal[k] * Kernel[n - k];
        }
    }
#ifdef OUTPUT
    printf("\n%d cycles for convolve\n",i);
#endif
}

void calculatePeaks( const double x[], size_t xl,
                    const double y[], size_t yl)
{
    int peak = true;
    double prev, diff;
    prev = x[0];
    for (int j = 1; j < xl; j++ )
    {
        diff = x[j] - prev;
        if ( peak )
        {
            if ( diff < 0 )
            {
                peak = false;
                xp[xpn++] = j;
            }
            prev = x[j];
            if (xpn == MAX_PEAKS) break;
        }
        else
        {
            if ( diff > 0 ) peak = true;
        }
    }
    
    prev = y[0];
    for (int i = 1; i < yl; i++ )
    {
        diff = y[i] - prev;
        if ( peak )
        {
            if ( diff < 0 )
            {
                peak = false;
                yp[ypn++] = i;
            }
            prev = y[i];
            if (ypn == MAX_PEAKS) break;
        }
        else
        {
            if ( diff > 0 ) peak = true;
        }
    }
}

void processImage( const char* input_file, const char* output_file, int iterations)
{
    
    double time[iterations];
    int width = 0;
    int height = 0;
    clock_t t1, t2;
    double diff;
    
    ifstream infile;
    ofstream outfile;
    size_t size = 0;
    
#ifdef OUTPUT
    cout << "Attempting to open " << INPUT_FILENAME << endl;
#endif
    
    infile.open( input_file, ios::in | ios::binary | ios::ate );
    outfile.open( output_file, ios::out | ios::binary | ios::ate );
    char* data = 0;
    infile.seekg (0, infile.end);
    int length = infile.tellg();
    
    infile.seekg(0);
    infile.seekg(18);
    char width_c[4];
    infile.read(width_c, 4);
    char height_c[4];
    infile.read(height_c, 4);
    std::stringstream wd;
    for(int i = 3 ; i >= 0; i--) wd << hex << ((int)width_c[i] & 0xff);
    wd >> width;
    std::stringstream ht;
    for(int i = 3 ; i >= 0; i--) ht << hex << 0xff - ((int)height_c[i] & 0xff);
    ht >> height;
    height++;
#ifdef OUTPUT
    cout << "Length is " << length << " with dimensions " << width << "x" << height << endl;
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
#endif
    
    infile.seekg(54);
    char image[height][width][4];
    
    double xi[width];
    double yi[height];
    
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            char pixel[4];
            infile.read(pixel, 4);
            
            if(pixel[0] < 0 && pixel[1] < 0 && pixel[2] < 0)
            {
                image[y][x][0] = -1;
                image[y][x][1] = 0;
                image[y][x][2] = 0;
                xi[x] += 1;
                yi[y] += 1;
            }
            else
            {
                image[y][x][0] = pixel[0];
                image[y][x][1] = pixel[1];
                image[y][x][2] = pixel[2];
            }
            
            image[y][x][3] = pixel[3];
        }
    }
    xcl = width + xkl - 1;
    double xc[xcl];
    ycl = height + ykl - 1;
    double yc[ycl];
    
    for(int l = 0; l < iterations; l++ )
    {
        t1 = clock();
        /* x convolution */
        
        convolve(xi, width, xk, xkl, xc);
        
        /* y convolution */
        
        convolve(yi, height, yk, ykl, yc);
        
        calculatePeaks(xc, width, yc, height);
        
        t2 = clock();
        
        diff = ((double)(t2 - t1) / 1000000.0F ) * 1000;
        time[l] = diff;
    }
    double timea = 0;
    for(int l = 0; l < iterations; l++ )
    {
        timea += time[l];
    }
    timea /= iterations;
    
    cout << "Detection took an average of " << timea << "ms over " << iterations << " iterations." << endl;
    
    for(int i = 0; i < ypn; i++ )
    {
        int Y = (int)((float)yp[i] * (float)height/(float)ycl);
        for(int j = 0; j < xpn; j++ )
        {
            int X = (int)((float)xp[j] * (float)width/(float)xcl);
#ifdef OUTPUT
            cout << "Density at (" << X << ", " << Y << ") is " << xc[X] + yc[Y] << endl;
#endif
            if(( xc[X] + yc[Y] ) > MIN_DENSITY )
            {
#ifdef OUTPUT
                cout << "Found point of interest at (" << X << ", " << Y << ")" << endl;
#endif
                
                image[Y][X][0] = 0;
                image[Y][X][1] = 0;
                image[Y][X][2] = -1;
                
                image[Y][X-1][0] = 0;
                image[Y][X-1][1] = 0;
                image[Y][X-1][2] = -1;
                
                image[Y][X+1][0] = 0;
                image[Y][X+1][1] = 0;
                image[Y][X+1][2] = -1;
                
                image[Y-1][X][0] = 0;
                image[Y-1][X][1] = 0;
                image[Y-1][X][2] = -1;
                
                image[Y+1][X][0] = 0;
                image[Y+1][X][1] = 0;
                image[Y+1][X][2] = -1;
                
                image[Y][X-2][0] = 0;
                image[Y][X-2][1] = 0;
                image[Y][X-2][2] = -1;
                
                image[Y][X+2][0] = 0;
                image[Y][X+2][1] = 0;
                image[Y][X+2][2] = -1;
                
                image[Y-2][X][0] = 0;
                image[Y-2][X][1] = 0;
                image[Y-2][X][2] = -1;
                
                image[Y+2][X][0] = 0;
                image[Y+2][X][1] = 0;
                image[Y+2][X][2] = -1;
            }
        }
    }
    
    infile.seekg(0);
    char byte[54];
    infile.read(byte, 54);
    outfile.write(byte, 54);
    
    int image_size = width*height*4;
    outfile.write((char *)&image, image_size);
#ifdef OUTPUT
    cout << "Output file is ready: " << output_file << endl;
#endif
    
}

int main(int argc, char *args[])
{
    double t;
    int iterations = 10;
    if(argc >= 2)
    {
        
        INPUT_FILENAME  = args[1];
        OUTPUT_FILENAME = "images/output_image.bmp";
        if(argc >= 3)
        {
            iterations = atoi(args[2]);
        }
    }
    cout << "Performing density mapping test for " << iterations << " iterations." << endl;
    processImage(INPUT_FILENAME, OUTPUT_FILENAME, iterations);
    
    
    return 0;
}
