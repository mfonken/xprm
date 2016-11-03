#include <iostream>
#include <fstream>
#include <cstring>
#include <cstddef>
#include <sstream>
#include "types.h"
#include "unfisheye.h"
#include "centroid.c"
#include <sys/time.h>

#define WIDTH  620
#define HEIGHT 387
#define IMG_T  150

#define OUTPUT

const char* INPUT_FILENAME = "images/kitten3.bmp";
const char* OUTPUT_FILENAME = "images/kitten_unfisheyed.bmp";

using namespace std;

double performTest( const char* input_file, const char* output_file, char inverse, char method)
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
    
    file.open( input_file, ios::in | ios::binary | ios::ate );
    outfile.open( output_file, ios::out | ios::binary | ios::ate );
    char* data = 0;
    file.seekg (0, file.end);
    int length = file.tellg();
    
    //    file.seekg(0, file.beg);
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
    
    
    file.seekg(54);
    char image[height][width][4];
    t1 = clock();
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            coord temp_c;
            temp_c.x = x;
            temp_c.y = y;
            
            switch( method )
            {
                case 'n':
                    if(inverse)
                        inverseFisheye(&temp_c, width, height);
                    else
                        fisheye(&temp_c, width, height);
                    break;
                case 'c':
                    if(inverse)
                        inverseFisheyeCordic(&temp_c, width, height);
                    else
                        fisheyeCordic(&temp_c, width, height);
                    break;
                case 's':
                    if(inverse)
                        inverseFisheyeStd(&temp_c, width, height);
                    else
                        fisheyeStd(&temp_c, width, height);
                    break;
                default:
                    break;
            }
            
            int new_x = temp_c.x;
            int new_y = temp_c.y;
            
            char pixel[4];
            file.read(pixel, 4);
            
            if(new_x >= 0 && new_x < width && new_y >= 0 && new_y < height)
            {
                image[new_y][new_x][0] = pixel[0];
                image[new_y][new_x][1] = pixel[1];
                image[new_y][new_x][2] = pixel[2];
                image[new_y][new_x][3] = pixel[3];
            }
        }
    }
    
    int num_blobs = 0;
    for(int y = 0; y < height * 0.9; y++)
    {
        char p[width];
        for(int x = 0; x < width; x++)
        {
            int pixel[3];
            for(int i = 0; i < 3; i++)
            {
                pixel[i] = (int)image[y][x][i];
            }
            if(pixel[0] < 0 && pixel[1] < 0 && pixel[2] < 0)
            {
                p[x] = 1;
            }
            else
            {
                p[x] = 0;
            }
        }
        
        num_blobs = getCentroids(p, width, y );
    }
    t2 = clock();
    diff = ((double)(t2 - t1) / 1000000.0F ) * 1000;
#ifdef OUTPUT
    cout << num_blobs << " centroids returned" << endl;
#endif
    for(int b = 0; b < num_blobs; b++)
    {
        int X = blobs[b].X;
        int Y = blobs[b].Y;
#ifdef OUTPUT
        cout << " " << b << ": (" << X << ", " << Y << ")" << endl;
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
    
    file.seekg(0, file.beg);
    char byte[54];
    file.read(byte, 54);
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

    char inverse = 1;
    char method = 'n';
    double t;
    if(argc >= 2)
    {
        
        INPUT_FILENAME  = args[1];
        OUTPUT_FILENAME = "output_image.bmp";
        if(argc >= 3)
        {
            inverse = atoi(args[2]);
            if(argc >= 4)
            {
                method = args[3][0];
                if(argc >= 5)
                {
                    OUTPUT_FILENAME = args[4];
                }
            }
        }
    }
    if(method == 'a')
    {
        t = performTest(INPUT_FILENAME, OUTPUT_FILENAME, inverse, 'n');
        cout << "Non-trig test performed in " << t << "ms" << endl;
        t = performTest(INPUT_FILENAME, OUTPUT_FILENAME, inverse, 'c');
        cout << "  Cordic test performed in " << t << "ms" << endl;
        t = performTest(INPUT_FILENAME, OUTPUT_FILENAME, inverse, 's');
        cout << "     Std test performed in " << t << "ms" << endl;
    }
    else
    {
        t = performTest(INPUT_FILENAME, OUTPUT_FILENAME, inverse, method);
        cout << "Test performed in " << t << "ms" << endl;
    }

    return 0;
}
