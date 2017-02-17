#include <iostream>
#include <fstream>
#include <cstring>
#include <cstddef>
#include <sstream>
#include "types.h"
#include "centroid.h"
#include <sys/time.h>

#define WIDTH  620
#define HEIGHT 387
#define IMG_T  150
#define MIN_M  150

#define OUTPUT

const char* INPUT_FILENAME = "images/kitten3.bmp";
const char* OUTPUT_FILENAME = "images/kitten_unfisheyed.bmp";

using namespace std;

double performTest( const char* input_file, const char* output_file)
{
    int width = WIDTH;
    int height = HEIGHT;
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
#endif
    
    infile.seekg(54);
    char image[height][width][4];
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
    resetBlobs();
    infile.seekg(54);
    t1 = clock();
    int num_centroids = 0;
    for(int y = 0; y < height; y++)
    {
        char p[width];
        for(int x = 0; x < width; x++)
        {
            if( image[y][x][0] < 0)// && image[y][x][1] > 0 && image[y][x][2] > 0)
            {
                p[x] = 1;
            }
            else
            {
                p[x] = 0;
            }
        }
        
        num_centroids = getCentroids(p, width, y);
    }
    t2 = clock();
    diff = ((double)(t2 - t1) / 1000000.0F ) * 1000;
#ifdef OUTPUT
    cout << num_centroids << " centroids returned" << endl;
#endif
    for(int b = 0; b < num_centroids; b++)
    {
        int X = centroids[b].X;
        int Y = centroids[b].Y;
        int M = centroids[b].mass;
        if( M > MIN_M )
        {
#ifdef OUTPUT
            cout << " " << b << ": m" << M << "(" << X << ", " << Y << ")" << endl;
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
    
    infile.seekg(0);
    char byte[54];
    infile.read(byte, 54);
    outfile.write(byte, 54);
    
    int image_size = width*height*4;
    outfile.write((char *)&image, image_size);
#ifdef OUTPUT
    cout << "Output file is ready: " << output_file << endl;
#endif
    
    return diff;
}

int main(int argc, char *args[])
{
    double t;
    if(argc >= 2)
    {
        
        INPUT_FILENAME  = args[1];
        OUTPUT_FILENAME = "images/output_image.bmp";
    }

    t = performTest(INPUT_FILENAME, OUTPUT_FILENAME);
    cout << "Test performed in " << t << "ms" << endl;
    
    return 0;
}
