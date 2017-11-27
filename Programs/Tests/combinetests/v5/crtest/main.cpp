//#define TILT_NONE
//#define TILT_P45
//#define TILT_N45
//#define TILT_P30
//#define TILT_N30
//#define TILT_P45_FIRST_QUAD_OFFSET
//#define TILT_N45_FIRST_QUAD_OFFSET
//#define TILT_N45_BOTTOM_OFFSET
//#define TILT_NONE_FIRST_QUAD_OFFSET
#define TILT_NONE_LEFT_OFFSET
//#define TILT_NONE_BOTTOM_OFFSET

#define Xc 640-188//265
#define Yc 0//53

#define A_ANGLE 105

//#define LOCK_BEACON_0
//#define LOCK_BEACON_1

#define DEBUG_ROTATION

#include <iostream>
#include <math.h>

#include "cr.hpp"
#include "kinetic_types.h"

double scale = 100;

#define WIDTH       1280
#define HEIGHT      800
#define HALF_WIDTH  WIDTH / 2
#define HALF_HEIGHT HEIGHT / 2
#define SCREEN_DIAG atan2(HALF_HEIGHT,HALF_WIDTH)

#define INC_DIV     100

#ifdef TILT_NONE
const char title[] = "no tilt & no offset";
const double default_rotation[] = {0,  0,0};
const int default_beaconA[]     = {320,400};
const int default_beaconB[]     = {960,400};
#endif
#ifdef TILT_P45
const char title[] = "+45º tilt & no offset";
const double default_rotation[] = {0, 45,0};
const int default_beaconA[]     = {414,626};
const int default_beaconB[]     = {866,174};
#endif
#ifdef TILT_N45
const char title[] = "-45º tilt & no offset";
const double default_rotation[] = {0,-45,0};
const int default_beaconA[]     = {414,174};
const int default_beaconB[]     = {866,626};
#endif
#ifdef TILT_P30
const char title[] = "+30º tilt & no offset";
const double default_rotation[] = {0, 30,0};
const int default_beaconA[]     = {360,238};
const int default_beaconB[]     = {921,562};
#endif
#ifdef TILT_N30
const char title[] = "-30º tilt & no offset";
const double default_rotation[] = {0,-30,0};
const int default_beaconA[]     = {360,562};
const int default_beaconB[]     = {921,238};
#endif
#ifdef TILT_P45_FIRST_QUAD_OFFSET
const char title[] = "+45º tilt & 1st quadrant offset";
const double default_rotation[] = {0, 45,0};
const int default_beaconA[]     = {-265+Xc,400+Yc};
const int default_beaconB[]     = {188+Xc,-53+Yc};
#endif
#ifdef TILT_N45_FIRST_QUAD_OFFSET
const char title[] = "-45º tilt & 1st quadrant offset";
const double default_rotation[] = {0,-45,0};
const int default_beaconA[]     = {-265+Xc,-53+Yc};
const int default_beaconB[]     = {188+Xc,400+Yc};
#endif
#ifdef TILT_N45_BOTTOM_OFFSET
const char title[] = "-45º tilt & 1st quadrant offset";
const double default_rotation[] = {0,-45,0};
const int default_beaconA[]     = {413+Xc,-53+Yc};
const int default_beaconB[]     = {866+Xc,400+Yc};
#endif
#ifdef TILT_NONE_FIRST_QUAD_OFFSET
const char title[] = "no tilt & 1st quadrant offset";
const double default_rotation[] = {0,0,0};
const int default_beaconA[]     = {0,HEIGHT/4};
const int default_beaconB[]     = {HALF_WIDTH,HEIGHT/4};
#endif
#ifdef TILT_NONE_LEFT_OFFSET
const char title[] = "no tilt & bottom half offset";
const double default_rotation[] = {0,0,0};
const int default_beaconA[]     = {0,400};
const int default_beaconB[]     = {640,400};
#endif
#ifdef TILT_NONE_BOTTOM_OFFSET
const char title[] = "no tilt & bottom half offset";
const double default_rotation[] = {0,0,0};
const int default_beaconA[]     = {320,200};
const int default_beaconB[]     = {960,200};
#endif

int main(int argc, const char * argv[])
{
//    qCT();
    printf("Running with %s.\n", title);
    Camera_Rotation_Init();
    
    kinetic_t kin;
    kin.rotation[0] = default_rotation[0] * DEG_TO_RAD;
    kin.rotation[1] = default_rotation[1] * DEG_TO_RAD;
    kin.rotation[2] = default_rotation[2] * DEG_TO_RAD;
    
    cartesian2_t bea[2];
    bea[0].x = default_beaconA[0];
    bea[0].y = default_beaconA[1];
    bea[1].x = default_beaconB[0];
    bea[1].y = default_beaconB[1];
    
    double dx = (bea[1].x-bea[0].x), dy = (bea[1].y-bea[0].y);
    printf("Beacons are #1(%4d,%4d) & #2(%4d,%4d)\n", (int)bea[0].x,(int)bea[0].y,(int)bea[1].x,(int)bea[1].y);
    printf("Angle is %.1fº and beacon distance is %.1f\n", -atan2(dy,dx)*RAD_TO_DEG, sqrt(dx*dx+dy*dy));
    ///NOTE: TILT IS INVERTED BECAUSE IT IS THE CAMERA'S TILT, NOT THE VISUALIZED BEACON VECTOR
    
#ifdef DEBUG_ROTATION
    char name = 'A';
    for(int i = A_ANGLE; name < 'H'; i -= 15)
    {
        printf("%c: %4dº>", name++, i);
        kin.rotation[2] = i * DEG_TO_RAD;
#else
    kin.rotation[2] = -90 * DEG_TO_RAD;
        
    double ang = kin.rotation[1], yt;
    bool lower = false;
    int lim = HALF_HEIGHT, inc = HALF_HEIGHT / INC_DIV;
    if(fabs(ang) < SCREEN_DIAG)
    {
        lower = true;
        lim = HALF_WIDTH;
        inc = HALF_WIDTH / INC_DIV;
    }
    printf("Angle %.2f[%.2f] is %s than diagonal %.2f\n", ang*RAD_TO_DEG, tan(ang), lower?"lower":"higher", SCREEN_DIAG*RAD_TO_DEG);
    for(int i = 0; i <= lim ; i+=inc)
    {
        if(lower)
        {
            bea[0].x = i;
            bea[1].x = WIDTH - bea[0].x;
            yt = HALF_HEIGHT + ((lim - i) * fabs(tan(ang)));
            if(ang < 0)
            {
                bea[1].y = yt;
                bea[0].y = HEIGHT - yt;
            }
            else
            {
                bea[0].y = yt;
                bea[1].y = HEIGHT - yt;
            }
        }
        else
        {
            bea[0].x = HALF_WIDTH - lim + (i * fabs(tan(ang)));
            bea[1].x = WIDTH - bea[0].x;
            yt = i;
            if(ang > 0)
            {
                bea[1].y = yt;
                bea[0].y = HEIGHT - yt;
            }
            else
            {
                bea[0].y = yt;
                bea[1].y = HEIGHT - yt;
            }
        }
#ifdef LOCK_BEACON_0
        bea[0].x = 640;
        bea[0].y = 400;
#endif
#ifdef LOCK_BEACON_1
        bea[1].x = 640;
        bea[1].y = 400;
#endif
        printf("(%4d,%4d)<>(%4d,%4d): ", (int)bea[0].x, (int)bea[0].y, (int)bea[1].x, (int)bea[1].y);
#endif
        
        cr(&kin, bea);
#ifdef PRINT_POS
        printf("\tX: %.4f Y: %.4f Z: %.4f (%s)\n", kin.position[0] * scale, kin.position[1] * scale, kin.position[2] * scale, UNITS);
#endif
    }
    
    return 0;
}
