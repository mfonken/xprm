/*! \file spatial.h
    \brief Spatial Orientation Main\r\n
 
 NOTES:
 - IMU is for rotational data ONLY!
 - Positional data comes from camera triangulation module and beacons
 - Right-handed rotation
 
  Created by Matthew Fonken on 10/8/16.
 */


#ifndef spatial_h
#define spatial_h

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <sys/time.h>

#include "kalman.h"
#include "matrix.h"

#define     sign( x )   ( x ) >= 0 ? 1:-1
#define     absl( x )   ( x ) >= 0 ? ( x ): -( x )

#define     MU                  0.1
#define     HALF_PI             3.141596 / 2

#define     VISION_ZSTATE_IHAT  0
#define     VISION_ZSTATE_JHAT  0
#define     VISION_ZSTATE_KHAT -1

#define     D_AUG               0
#define     BEACON_DISTANCE     1

#define     I2C_BUFFER_LENGTH   14
#define     IMU_ADDR            0xd0

#define     VISION_WIDTH        640
#define     VISION_HEIGHT       480
#define     VISION_CENTER_X     VISION_WIDTH  / 2
#define     VISION_CENTER_Y     VISION_HEIGHT / 2

//#define     DEBUG

/* Position -
    [0] - X
    [1] - Y
    [2] - Z
 
   Rotation -
    [0] - φ > around X
    [1] - θ > around Y
    [2] - ψ > around Z
 */

struct cartesian2   vis[2];

struct kalman   pos_f[3];
struct kalman   rot_f[3];
struct kalman   tru_f[3];
double          pos[3];
double          rot[3];
double          tru[3];

uint16_t        acc[3];
uint16_t        gyro[3];
uint16_t        temperature;

double          delta_t;

uint8_t         i2c_data[I2C_BUFFER_LENGTH];

/*! Distance between to Cartesian coordinates
 \f{eqnarray*}{
 d = \sqrt{(x_b^2 - x_a^2) + (y_b^2 - y_a^2)}
 \f}
 */
uint32_t get2dDistance( struct cartesian2 *a, struct cartesian2 *b )
{
    uint32_t a_x = a->x;
    uint32_t a_y = a->y;
    uint32_t b_x = b->x;
    uint32_t b_y = b->y;
    
    return sqrt( ( ( b_x * b_x ) - ( a_x * a_x ) ) + ( ( b_y * b_y ) - ( a_y * a_y ) ) );
}

uint8_t i2cRead( uint8_t addr, uint8_t *reg)
{
    uint8_t data = 0;
    return data;
}
void i2cWrite( uint8_t addr, uint8_t reg, uint8_t val )
{
    return;
}

/*! Get IMU data using I2C */
void getIMU( void )
{
    while (i2cRead(0x3B, i2c_data));
    acc[0]      = ( ( i2c_data[0]  << 8 ) | i2c_data[1] );
    acc[1]      = ( ( i2c_data[2]  << 8 ) | i2c_data[3] );
    acc[2]      = ( ( i2c_data[4]  << 8 ) | i2c_data[5] );
    temperature = (   i2c_data[6]  << 8 ) | i2c_data[7];
    gyro[0]     = (   i2c_data[8]  << 8 ) | i2c_data[9];
    gyro[1]     = (   i2c_data[10] << 8 ) | i2c_data[11];
    gyro[2]     = (   i2c_data[12] << 8 ) | i2c_data[13];
}

/*! Set IMU register using I2C */
void setIMU( uint8_t reg, uint8_t val )
{
    i2cWrite(IMU_ADDR, reg, val);
}
void movEvent();
void setCursor();
void getCursor();

/* See - http://www.nxp.com/files/sensors/doc/app_note/AN3461.pdf */

/*! Get roll from IMU data */
double getRoll()
{
    return atan( acc[1] / ( sign( acc[2] ) * sqrt( ( acc[2] * acc[2] ) + ( MU * ( acc[0] * acc[0] ) ) ) ) ); // Eqn. 38
}

/*! Get pitch from IMU data */
double getPitch()
{
    return atan( -acc[0] / sqrt( ( acc[1] * acc[1] ) + ( acc[2] * acc[2] ) ) ); // Eqn. 37
}

/*! Get yaw from IMU data */
double getYaw()
{
    return atan( -acc[2] / sqrt( ( acc[0] * acc[0] ) + ( acc[1] * acc[1] ) ) ); // Eqn. 37
}

/*! Find acceleration apart from gravity */
struct vec3 * getNonGravAcceleration()
{
    /* Tait-Bryan angles of vision */
    struct ang3 tba;
    tba.a = rot_f[0].value;
    tba.b = rot_f[1].value;
    tba.c = rot_f[2].value;
    
    /* Create a vector of accelerometer values */
    struct vec3 avec;
    avec.ihat = acc[0];
    avec.jhat = acc[1];
    avec.khat = acc[2];
    
    /* Transform and normalize v vector by given angles to get unit vector from camera */
    struct vec3 * atru = zxyTransform( &avec, &tba, 1 );
    
    atru->khat += 1; // Negate gravity
    return atru;
}

/*! Filter Initializer */
void initFilters( void )
{
    initKalman( &rot_f[0], getRoll()  );
    initKalman( &rot_f[1], getPitch() );
    initKalman( &rot_f[2], getYaw()   );
}

/*! IMU Initializer */
uint8_t initIMU( void )
{
    i2cRead(0x75, i2c_data);
    if (i2c_data[0] != 0x68) { // Read "WHO_AM_I" register
        
#ifdef DEBUG
        cout << ("Error reading sensor");
#endif
        return 1;
    }
    getIMU();
    initFilters();
    
    return 0;
}

/*! IMU even handler */
void imuEvent()
{
    getIMU();
    
    uint32_t θ = getPitch();
    uint32_t φ = getRoll();
    uint32_t ψ = getYaw();
    
    uint32_t kalman_temp = rot_f[0].value;
    if ((θ < -HALF_PI && kalman_temp > HALF_PI) || (θ > HALF_PI && kalman_temp < -HALF_PI))
    {
        rot_f[1].value  = θ;//
        rot[1]          = θ;
    }
    else
    {
        updateKalman( &rot_f[1], θ, gyro[1], delta_t ); // Calculate the true pitch using a Kalman filter
        rot[1] = rot_f[1].value;
    }

    kalman_temp = absl(rot_f[1].value);
    if (kalman_temp > HALF_PI)
    {
        gyro[0] = -gyro[0]; // Invert rate, so it fits the restriced accelerometer reading
    }
    updateKalman( &rot_f[0], φ, gyro[0], delta_t ); // Calculate the true roll using a Kalman filter
    rot[0] = rot_f[0].value;

    updateKalman( &rot_f[2], ψ, gyro[2], delta_t ); // Calculate the true yaw using a Kalman filter
}

/*! Get data from vision module */
void getVision()
{
    return;
}

/*! Vision module event handler */
void visionEvent()
{
    getVision(); // Update vis[] array
}

/*! Augment vision data
 \f{eqnarray*}{
    &\mathbf{v} =
    \begin{cases}
        &v_{\hat{i}} = VZ_{\hat{i}} \\
        &v_{\hat{j}} = VZ_{\hat{j}} \\
        &v_{\hat{k}} = VZ_{\hat{k}} \\
    \end{cases} \\
    &\mathbf{v_{true}} = \text{zxyTranform}(\mathbf{v}, \mathbf{a}, 1) \\
    &\mathbf{d_{true}} = \text{zxyTranform}(\mathbf{d}, \mathbf{a}, 1) \\
    &c_{augment} = \frac{D_{beacon}(1 + D_{augment})}{||\mathbf{d_{true}}||} \\
    &\mathbf{v_{return}} =c_{augment}\mathbf{v_{true}}
 \f}
 */
struct vec3 * dAugment(struct vec3 * dvec, struct ang3 * a)
{
    /* Create v vector for zero state */
    struct vec3 vvec;       // Vision system vector
    vvec.ihat = VISION_ZSTATE_IHAT;
    vvec.jhat = VISION_ZSTATE_JHAT;
    vvec.khat = VISION_ZSTATE_KHAT; // Defualt is (0,0,-1) or facing down (towards center of gravity)
    //normalizeVec3(vvec); // normalize if vvec inital isn't of length 1
    /* Transform and normalize v vector by given angles to get unit vector from camera */
    struct vec3 *vtru = zxyTransform( &vvec, a, 0 );
    
    /* Transform d vector by given angles to get true vector between beacons */
    struct vec3 *dtru = zxyTransform( dvec, a, 0 );
    
    /* Calculate estimated augmentation of v vector 
     by ratio of transformed true d estimation and real (BEACON_DISTANCE) */
    double aug = ( BEACON_DISTANCE * ( 1 + D_AUG ) ) / lengthOfVec3( dtru ) ;
    
    /* Apply augmentation and return */
    vtru->ihat *= aug;
    vtru->jhat *= aug;
    vtru->khat *= aug;
    return vtru;
}

/*! Get absolute position 
 \f{eqnarray*}{
    &a_a = rot_{f_0}, a_b = rot_{f_1}, a_c = rot_{f_2} \\
 
    &\mathbf{d} =
    \begin{cases}
        &d_{\hat{i}} = vis_{1_x} - vis_{0_x} \\
        &d_{\hat{j}} = vis_{1_y} - vis_{0_y} \\ 
        &d_{\hat{k}} = 0 \\
    \end{cases} \\
    &\mathbf{r} = \text{dAugment}(\mathbf{d}, \mathbf{a}) \\
    &\mathbf{e} =
    \begin{cases}
        &e_{\hat{i}} = V_{center_x} - vis_{0_x} \\
        &e_{\hat{j}} = V_{center_y} - vis_{0_y} \\
        &e_{\hat{k}} = 0 \\
    \end{cases} \\
    &\mathbf{e_{true}} = \text{zxyTranform}(\mathbf{e}, \mathbf{a}, 1) \\
    &\mathbf{p_{true}} = \mathbf{r} - \mathbf{e_{true}} \\
    &\mathbf{n} = \text{getNonGravAcceleration()} \\
    &\mathbf{v} =
    \begin{cases}
        &v_x = &n_{\hat{i}}\Delta{t} \\
        &v_y = &n_{\hat{j}}\Delta{t} \\
        &v_z = &n_{\hat{k}}\Delta{t} \\
    \end{cases} \\
    &\text{Update all position Kalmans with } \mathbf{p_{true}}, \mathbf{v}, \text{ and } \Delta{t}
 \f}
 */
void getAbsolutePosition()
{
    /* Tait-Bryan angles of vision */
    struct ang3 tba;
    tba.a = rot_f[0].value;
    tba.b = rot_f[1].value;
    tba.c = rot_f[2].value;
    
    /* vector from B1 in vision -TO-> B2 in vision */
    struct vec3 dvec;
    dvec.ihat = vis[1].x - vis[0].x;
    dvec.jhat = vis[1].y - vis[0].y;
    dvec.khat = 0;                             // d is only on XY plane (surface of beacons)
    
    /* Create r vector (camera -TO-> vision center) from augment generated true d and vision d */
    struct vec3 rvec = *( dAugment( &dvec, &tba ) );
    
    /* vector between vision center -TO-> B1 */
    struct vec3 evec;
    evec.ihat = VISION_CENTER_X - vis[0].x;
    evec.jhat = VISION_CENTER_Y - vis[0].y;
    evec.khat = 0;                             // e is only on XY plane (surface of beacons)
    
    /* vector between vision center -TO-> B2 */
    struct vec3 fvec;
    fvec.ihat = VISION_CENTER_X - vis[1].x;
    fvec.jhat = VISION_CENTER_Y - vis[1].y;
    fvec.khat = 0;                             // f is only on XY plane (surface of beacons)
    
    /* Transform e vector (vision center -TO-> B1) to true e vector (transformed vision center -TO-> true B1) */
    struct vec3 etru = *( zxyTransform( &evec, &tba, 1 ) );
    
    /* Subract true e vector from augmented r vector */
    subtractVec3s(&rvec, &etru);
    
    tru[0] = rvec.ihat;
    tru[1] = rvec.jhat;
    tru[2] = rvec.khat;
    
    /* Filter calculated absolute position and with integrated acceleration (velocity) */
    struct vec3 ngacc = *( getNonGravAcceleration() );
    double delta_time = ((double)(clock() - tru_f[0].timestamp) / 1000000.0F ) * 1000;
    double x_vel = ngacc.ihat * delta_time;
    updateKalman( &tru_f[0], tru[0], x_vel, delta_time);
    double y_vel = ngacc.jhat * delta_time;
    updateKalman( &tru_f[1], tru[1], y_vel, delta_time);
    double z_vel = ngacc.khat * delta_time;
    updateKalman( &tru_f[2], tru[2], z_vel, delta_time);
}

#endif /* spatial_h */
