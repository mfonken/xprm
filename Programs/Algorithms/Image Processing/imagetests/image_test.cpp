#include <iostream>
#include <fstream>
#include <cstring>
#include <cstddef>
#include <sstream>
#include "unfisheye.h"

#define WIDTH  620
#define HEIGHT 387

const char* INPUT_FILENAME = "images/kitten3.bmp";
const char* OUTPUT_FILENAME = "images/kitten_unfisheyed.bmp";

using namespace std;

int main(int argc, char *args[])
{
    int width = WIDTH;
    int height = HEIGHT;
    char inverse = 1;
    if(argc >= 2)
    {
        
        INPUT_FILENAME  = args[1];
        OUTPUT_FILENAME = "images/output_image.bmp";
        if(argc >= 3)
        {
            inverse = atoi(args[2]);
            if(argc == 4)
            {
               OUTPUT_FILENAME = args[3];
            }
        }
    }
    
    ifstream file;
    ofstream outfile;
    size_t size = 0;

    cout << "Attempting to open " << INPUT_FILENAME << endl;

    file.open( INPUT_FILENAME, ios::in | ios::binary | ios::ate );
    outfile.open( OUTPUT_FILENAME, ios::out | ios::binary | ios::ate );
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
    cout << "Length is " << length << " with dimensions " << width << "x" << height << endl;
    
    
    file.seekg(54);
    char image[height][width][4];
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            coord temp_c;
            temp_c.x = x;
            temp_c.y = y;
            
            if(inverse)
                inverseFisheye(&temp_c, width, height);
            else
                fisheye(&temp_c, width, height);
            
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
    
    file.seekg(0, file.beg);
    char byte[54];
    file.read(byte, 54);
    outfile.write(byte, 54);
    
    int image_size = width*height*4;
    outfile.write((char *)&image, image_size);
    

    cout << "Output file is ready: " << OUTPUT_FILENAME << endl;
    
    return 0;
}
