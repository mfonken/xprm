#include <iostream>
#include <fstream>
#include <cstring>
#include <cstddef>
#include <sstream>
#include <math.h>
#include <sys/time.h>
#include "centroid.c"

#define WIDTH  200
#define HEIGHT 200

#define ITERATIONS 100

#define INTERVAL 1

//#define OUTPUT

const char* INPUT_FILENAME =
//"images/star_large.bmp";
"images/single_noise_large.bmp";
//"images/ellipse_large.bmp";
const char* OUTPUT_FILENAME = "output.bmp";

const int i_orders[] = { 10, 100, 1000, 10000, 100000 };
int i_o_n = 5;

using namespace std;

static inline double timeDiff( struct timeval a, struct timeval b ) { return ((b.tv_sec  - a.tv_sec) + (b.tv_usec - a.tv_usec)/1000000.0); }// + 0.0005; }

int main( int argc, char * args[])
{
    
    int width = WIDTH;
    int height = HEIGHT;
    clock_t t1, t2;
    double diff;
    
    ifstream file;
    ofstream outfile;
    size_t size = 0;
#ifdef OUTPUT
    cout << "Attempting to open " << INPUT_FILENAME << endl;
#endif
    printf("Analyzing %s\n", INPUT_FILENAME);
    file.open( INPUT_FILENAME, ios::in | ios::binary | ios::ate );
    outfile.open( OUTPUT_FILENAME, ios::out | ios::binary | ios::ate );
    char* data = 0;
    file.seekg (0, file.end);
    int length = file.tellg();
    
    file.seekg(0);
    file.seekg(18);
    char width_c[4];
    file.read(width_c, 4);
    char height_c[4];
    file.read(height_c, 4);
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

    t1 = clock();
    file.seekg(0);
    char header[54];
    file.read(header, 54);
    
    outfile.write(header, 54);
    char image[height][width][4];
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            char pixel[4];
            file.read(pixel, 4);
            for(int p = 0; p < 4; p++)
            {
                image[y][x][p] = pixel[p];
            }
        }
    }
    
    
    initCentroids(width, height);
    
    double t[100000];
    struct timeval a,b;
    for(int o = 0; o < i_o_n; o++)
    {
        for(int i = 0; i < i_orders[o]; i++)
        {
            gettimeofday( &a, NULL);
            for(int y = 0; y < height; y++)
            {
                
        #ifdef OUTPUT
                cout << "Performing Centroids on line #" << y << endl;
        #endif
                char line[width];
                for(int x = 0; x < width; x++)
                {
                    int br = sqrt(image[y][x][0] * image[y][x][0] + image[y][x][1] * image[y][x][1] + image[y][x][2] * image[y][x][2]);
        #ifdef OUTPUT
                   // cout << x << "-" << br << "|";
        #endif
                    line[x] = br;
                }
        #ifdef OUTPUT
                //cout << endl;
        #endif
                
                
                getCentroids((uint8_t *)line, y, INTERVAL);
                
            }
            gettimeofday( &b, NULL);
            t[i] = timeDiff(a,b);
        }
        long double avg = 0;
        for(int i = 0; i < i_orders[o]; i++) avg += t[i];
        avg /= (double)i_orders[o];
        printf("%d, - %Lf (s)\n", i_orders[o], avg);
    }
    
    
    int n = centroids.numBlobs;

    cout << "Found " << n << " blobs:" << endl;
    
    for(int i = 0; i < n; i++)
    {
        blob_t b = centroids.blobs[i];
        int x = (int)b.X;
        int y = (int)b.Y;
        char red[4] = {0, 0, 255, 0};
        for(int p = 0; p < 4; p++)
        {
            image[y-1][x][p] = red[p];
            image[y+1][x][p] = red[p];
            image[y][x-1][p] = red[p];
            image[y][x+1][p] = red[p];
            image[y][x][p] = red[p];
        }
//        cout << ">(" << x << ", " << y << ")<" << endl;
    }
    
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            outfile.write(image[y][x], 4);
        }
    }
    
    cout << "Output file is ready." << endl;
    return 0;
}
