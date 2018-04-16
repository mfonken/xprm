/*! \file spatial.h
    \brief Spatial Orientation Main\r\n
 
 NOTES:
 - IMU is for rotational data ONLY!
 - Positional data comes from camera triangulation module and beacons
 - Right-handed rotation
 
  Created by Matthew Fonken on 10/8/16.
 */


#ifndef kinetic_h
#define kinetic_h

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <sys/time.h>

#include "em_i2c.h"
#include "cartesian.h"
#include "kalman.h"
#include "matrix.h"
#include "imu.h"
#include "vision.h"

#define     MU                  0.1
#define     HALF_PI             3.141596 / 2

#define     VISION_ZSTATE_IHAT  0
#define     VISION_ZSTATE_JHAT  0
#define     VISION_ZSTATE_KHAT -1

#define     D_AUG               0
#define     BEACON_DISTANCE     1

#define     VISION_WIDTH        640
#define     VISION_HEIGHT       480
#define     VISION_CENTER_X     VISION_WIDTH  / 2
#define     VISION_CENTER_Y     VISION_HEIGHT / 2

cartesian2_t   vis[2];
double         delta_t;

/*! Kinetic Type */
typedef struct _kinetic_t
{
    double     pos[3];
    double     rot[3];
    double     tru[3];
    
    kalman_t   pos_f[3];
    kalman_t   rot_f[3];
    kalman_t   tru_f[3];
} kinetic_t;
kinetic_t      kinetics;

/*! Filter Initializer */
void initFilters( void )
{
    initkalman_t( &kinetics.rot_f[0], getRoll()  );
    initkalman_t( &kinetics.rot_f[1], getPitch() );
    initkalman_t( &kinetics.rot_f[2], getYaw()   );
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
vec3_t *dAugment( vec3_t *dvec,
                  ang3_t *a)
{
    /* Create v vector for zero state */
    vec3_t vvec;       // Vision system vector
    vvec.ihat = VISION_ZSTATE_IHAT;
    vvec.jhat = VISION_ZSTATE_JHAT;
    vvec.khat = VISION_ZSTATE_KHAT; // Defualt is (0,0,-1) or facing down (towards center of gravity)
    //normalizevec3_t(vvec); // normalize if vvec inital isn't of length 1
    /* Transform and normalize v vector by given angles to get unit vector from camera */
    vec3_t *vtru = zxyTransform( &vvec, a, 0 );
    
    /* Transform d vector by given angles to get true vector between beacons */
    vec3_t *dtru = zxyTransform( dvec, a, 0 );
    
    /* Calculate estimated augmentation of v vector 
     by ratio of transformed true d estimation and real (BEACON_DISTANCE) */
    double aug = ( BEACON_DISTANCE * ( 1 + D_AUG ) ) / lengthOfvec3_t( dtru ) ;
    
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
    &\text{Update all position kalman_ts with } \mathbf{p_{true}}, \mathbf{v}, \text{ and } \Delta{t}
 \f}
 */
void getAbsolutePosition()
{
    /* Tait-Bryan angles of vision */
    ang3_t tba;
    tba.a = kinetics.rot_f[0].value;
    tba.b = kinetics.rot_f[1].value;
    tba.c = kinetics.rot_f[2].value;
    
    /* vector from B1 in vision -TO-> B2 in vision */
    vec3_t dvec;
    dvec.ihat = kinetics.vis[1].x - kinetics.vis[0].x;
    dvec.jhat = kinetics.vis[1].y - kinetics.vis[0].y;
    dvec.khat = 0;                             // d is only on XY plane (surface of beacons)
    
    /* Create r vector (camera -TO-> vision center) from augment generated true d and vision d */
    vec3_t rvec = *( dAugment( &dvec, &tba ) );
    
    /* vector between vision center -TO-> B1 */
    vec3_t evec;
    evec.ihat = VISION_CENTER_X - kinetics.vis[0].x;
    evec.jhat = VISION_CENTER_Y - kinetics.vis[0].y;
    evec.khat = 0;                             // e is only on XY plane (surface of beacons)
    
    /* vector between vision center -TO-> B2 */
    vec3_t fvec;
    fvec.ihat = VISION_CENTER_X - kinetics.vis[1].x;
    fvec.jhat = VISION_CENTER_Y - kinetics.vis[1].y;
    fvec.khat = 0;                             // f is only on XY plane (surface of beacons)
    
    /* Transform e vector (vision center -TO-> B1) to true e vector (transformed vision center -TO-> true B1) */
    vec3_t etru = *( zxyTransform( &evec, &tba, 1 ) );
    
    /* Subract true e vector from augmented r vector */
    subtractvec3_ts(&rvec, &etru);
    
    kinetics.tru[0] = rvec.ihat;
    kinetics.tru[1] = rvec.jhat;
    kinetics.tru[2] = rvec.khat;
    
    /* Filter calculated absolute position and with integrated acceleration (velocity) */
    vec3_t ngacc = *( getNonGravAcceleration() );
    double delta_time = ((double)(clock() - kinetics.tru_f[0].timestamp) / 1000000.0F ) * 1000;
    double x_vel = ngacc.ihat * delta_time;
    updatekalman_t( &kinetics.tru_f[0], kinetics.tru[0], x_vel, delta_time);
    double y_vel = ngacc.jhat * delta_time;
    updatekalman_t( &kinetics.tru_f[1], kinetics.tru[1], y_vel, delta_time);
    double z_vel = ngacc.khat * delta_time;
    updatekalman_t( &kinetics.tru_f[2], kinetics.tru[2], z_vel, delta_time);
}

#endif /* kinetic_h */
