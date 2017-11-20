#include "test_functions.h"
#include "image_processing.h"

#include <iostream>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define IMAGE_ROOT "/Users/matthewfonken/Desktop/test images/"

#define THRESHOLD   245
#define BRIGHTNESS  100
#define DRAW_HEIGHT 200

using namespace cv;
using namespace std;

Vec3b red = {0,0,255};
Vec3b yellow = {0,255,255};
Vec3b lime = {100,255,0};
Vec3b green = {0,255,0};

void drawArray( string name, FLOAT * arr, INT len, Mat O, Vec3b color )
{
    
    INT w = len, h = O.rows;
    Vec3b red   = {   0,   0, 255};

    for(INT i = 0; i < w; i++)
    {
        FLOAT c = arr[i]*SCALE;
//        printf("%d(%2.2f)\n",i,c);
        if(c >= h) c = h;
        if(c < 1) c = 1;
        O.at<Vec3b>(h-c, i) = color;
    }
}

void drawDensityMap(density_map_t * m, Mat O, Vec3b color)
{
    INT l = O.cols;
    INT z = O.rows;

    INT d = 0;
    
    for(INT i=0; i<l; i++)
    {
        d = m->map[i];
        if(d >= z) d = z-1;
        if(d < 0) d = 0;
        O.at<Vec3b>(z-1-d,i) = color;
    }
    
}

bool thresh(INT t, Vec3d p)
{
    /* Check first BGR channel only */
    return p[0] > t;
}

void MatToCimage( INT width, INT height, Mat mat, pixel_base_t ** img )
{
    for(INT y = 0; y < height; y++)
    {
        for(INT x = 0; x < width; x++)
        {
            Vec3b pixel = mat.at<Vec3b>(y, x);
            img[y][x] = (thresh(THRESHOLD, pixel)) ? 1:0;
        }
    }
}
void cimageToMat( INT width, INT height, pixel_base_t ** img, Mat mat )
{
    for(INT y = 0; y < height; y++)
    {
        for(INT x = 0; x < width; x++)
        {
            Vec3b pixel;
            pixel[0] = img[y][x];
            pixel[1] = img[y][x];
            pixel[2] = img[y][x];
            mat.at<Vec3b>(y, x) = pixel;
        }
    }
}
void cimageToBandW( INT width, INT height, pixel_base_t ** img, Mat mat )
{
    for(INT y = 0; y < height; y++)
    {
        for(INT x = 0; x < width; x++)
        {
            Vec3b pixel;
            pixel[0] = img[y][x]*BRIGHTNESS;
            pixel[1] = img[y][x]*BRIGHTNESS;
            pixel[2] = img[y][x]*BRIGHTNESS;
            mat.at<Vec3b>(y, x) = pixel;
        }
    }
}

void initImg( pixel_base_t *** i, INT w, INT h )
{
    *i = (pixel_base_t**)malloc(sizeof(pixel_base_t*) * h);
    for(INT y=0;y<h;y++) (*i)[y] = (pixel_base_t*)malloc(sizeof(pixel_base_t) * w);
}

void initDensityMapPair( density_map_pair_t * m, INT w, INT h )
{
    (*m).x.map = (FLOAT*)malloc(sizeof(FLOAT)*h);
    (*m).x.vel = (FLOAT*)malloc(sizeof(FLOAT)*h);
    (*m).x.acc = (FLOAT*)malloc(sizeof(FLOAT)*h);
    (*m).x.jrk = (FLOAT*)malloc(sizeof(FLOAT)*h);
    (*m).x.jnc = (FLOAT*)malloc(sizeof(FLOAT)*h);
    (*m).x.length = 0;
    (*m).y.map = (FLOAT*)malloc(sizeof(FLOAT)*w);
    (*m).y.vel = (FLOAT*)malloc(sizeof(FLOAT)*w);
    (*m).y.acc = (FLOAT*)malloc(sizeof(FLOAT)*w);
    (*m).y.jrk = (FLOAT*)malloc(sizeof(FLOAT)*w);
    (*m).y.jnc = (FLOAT*)malloc(sizeof(FLOAT)*w);
    (*m).y.length = 0;
}

INT main(INT argc, const char * argv[]) {
    Mat image, frame;
    
    INT l = 11;
    FLOAT s = 1.0;
    if(argc >= 2) l = atoi(argv[1]);
    if(argc >= 3) s = atof(argv[2]);

    std::string file(IMAGE_ROOT);
    if(argc >= 4) file.append(argv[3]);
    else file.append("frames/ellipse/1.png");
    printf("opening file: %s\n", file.c_str());
    image = imread(file, IMREAD_COLOR );
    if( image.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    INT width  = image.cols;
    INT height = image.rows;
    Mat out(width, height, CV_8UC3, Scalar(245,245,245));
    pixel_base_t **pixels;
    initImg(&pixels, width, height);
    
    MatToCimage(width, height, image, pixels);
    //cimageToBandW( width, height, pixels, out );
    
    printf("Generating gaussian of length %d and sigma %.1f:\n", l, s);
    gaussian_t g;
    initGaussian(&g, MAX_LEN);
    generateGaussian(&g, l, s);
    
    while(true)
    {
        Mat O(DRAW_HEIGHT, width, CV_8UC3, Scalar(0,0,0));
        drawArray("Test", g.array, g.length, O, lime );
        density_map_pair_t d1, d2;
        initDensityMapPair(&d1, width, height);
        initDensityMapPair(&d2, width, height);
        generateDensityMap( pixels, width, height, &d1 );
        generateDensityMap( pixels, width, height, &d2 );
        drawDensityMap(&d1.x, O, yellow);
        
        convolve( d1.x.map, d1.x.length, g.array, g.length );
        drawDensityMap(&d1.x, O, green);
    
        crossCorrelate( d2.x.map, d2.x.length, g.array, g.length );
        drawDensityMap(&d2.x, O, red);
        imshow("Density Map", O);
        waitKey(50000);
    }
    return 0;
}
