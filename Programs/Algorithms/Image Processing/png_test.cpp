#include <iostream>
#include <fstream>
#include <cstring>
#include <cstddef>

const char* INPUT_FILENAME = "images/kitten.png";

using namespace std;

int main(int argc, char *args[])
{
    if(argc == 2)
    {
        INPUT_FILENAME = args[1];
    }
    ifstream file;
    size_t size = 0;

    cout << "Attempting to open " << INPUT_FILENAME << endl;

    file.open( INPUT_FILENAME, ios::in | ios::binary | ios::ate );
    char* data = 0;

// skip the chunk
    file.seekg (0, file.end);
    int length = file.tellg();
    cout << "Length is " << length << endl;
    file.seekg(0);
    for(int i = 0; i < 54; i+=4)
    {
        
        char out_byte[4];
        file.read( out_byte, 4 );
        cout << "b" << i << ":\t";
        for(int j = 0; j < 4; j++)
        {
            cout << hex << (int)(out_byte[j]);
        }
        cout << "\n";
    }
    cout << "|" << endl;
    file.seekg(18);
    char *width = new char[5];
    file.read( width, 4 );
    width[ 4 ] = '\0';
    int w = 0;
    cout << "Width: ";
    for(int i = 4; i >= 0; i--) {
        w += (+width[i] & 0xff) ;
        w = w << 8;
        cout << hex << (+width[i] & 0xff);
    }
    w = w >> 8;
    cout << dec << "-> dec: " << w ;
    cout << endl;

    char *height = new char[5];
    file.read( height, 4 );
    height[ 4 ] = '\0';
    int h = 0;
    cout << "Height: ";
    for(int i = 4; i >= 0; i--) {
        h += (+height[i] & 0xff);
        h = h << 8;
        cout << hex << (+height[i] & 0xff);
    }
    h = h >> 8;
    cout << dec << "-> dec: " << h ;
    cout << endl;

    cout << "The image is size " << w << "x" << h << endl;


}