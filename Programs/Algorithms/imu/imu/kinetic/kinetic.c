/***********************************************************************************************//**
 * \file   k->c
 * \brief  Kinetic Motion code
 ***************************************************************************************************
 *  Created on: Nov 13, 2016
 *      Author: Matthew Fonken
 **************************************************************************************************/

/* Own header */
#include "kinetic.h"

/***************************************************************************************************
 Local Variables
 **************************************************************************************************/
/** Local change in time */
/** Local positional and rotational vectors */

/***********************************************************************************************//**
 *  \brief  Initialize Kinetic Sensors
 **************************************************************************************************/
int init( kinetic_t * k )
{
    Camera_Rotation_Init(k);
    return 1;
}

void updateRotation( kinetic_t * k, ang3_t e, ang3_t g )
{
    /* Restrict pitch */
    double v = k->filters.rotation[0]->value;
    if( ( e.x < -M_PI_2 && v >  M_PI_2 ) ||
       ( e.x >  M_PI_2 && v < -M_PI_2 ) )
    {
        k->filters.rotation[0]->value  = e.x;
        k->values.rotation[0] = e.x;
    }
    else
    {
        /* Calculate the true pitch using a kalman_t filter */
        Kalman.update( k->filters.rotation[0], e.x, g.x );
        k->values.rotation[0] = k->filters.rotation[0]->value;
    }
    
    if ( k->values.rotation[0] > 90 )
    {
        /* Invert rate, so it fits the restricted accelerometer reading */
        g.x = -g.x;
    }
    /* Calculate the true roll using a kalman_t filter */
    Kalman.update( k->filters.rotation[1], e.y, g.y );
    k->values.rotation[1] = k->filters.rotation[1]->value;
    
    /* Calculate the true yaw using a kalman_t filter */
    Kalman.update( k->filters.rotation[2], e.z, g.z );
    k->values.rotation[2] = k->filters.rotation[2]->value;
}

void updatePosition( kinetic_t * k, double beaconA[2], double beaconB[2] )
{
    /*** LOCAL VARIABLES ***/
    mat3x3_t m;
    
    /* Local angles */
    ang3_t a, r;
    double the_B, the_A, phi_B, phi_A, psi_r, sigma_A, sigma_r, nu, up;
    
    /* Local lengths */
    double x_B, x_A, y_B, y_A, d_x, d_y, A_l_sq, d__l, n_l, u_l, r_l;
    
    /*** CALCULATING QUATERNIONS ***/
    
    x_A = beaconA[0] - CAMERA_HALF_WIDTH;
    y_A = beaconA[1] - CAMERA_HALF_HEIGHT;
    x_B = beaconB[0] - CAMERA_HALF_WIDTH;
    y_B = beaconB[1] - CAMERA_HALF_HEIGHT;
    
    d_x = x_A - x_B;
    d_y = y_A - y_B;
    
    /* Get pixel distance from (0,0) to beacon 1 */
    A_l_sq = ( ( x_A * x_A ) + ( y_A * y_A ) );
    
    /* Get unit distance between beacons */
    d__l = sqrt( ( d_x * d_x ) + ( d_y * d_y ) ) * PIXEL_TO_UNIT;
    
    
    /* Get beacon angles */
    the_B   = atan2( ( x_B * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    phi_B   = atan2( ( y_B * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    the_A   = atan2( ( x_A * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    phi_A   = atan2( ( y_A * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    psi_r   = atan2( d_y, d_x );
    
    /* Get angles between beacons */
    ///NOTE: acos(cos()cos()) only works for combining frame-perpendicular vectors (i.e. theta & phi) [ang3 is used instead for sigma_r]
    sigma_A = acos( cos( the_A ) * cos( phi_A ) );
    
    vec3_t A, B;
    A.i = x_B;
    A.j = y_B;
    A.k = FOCAL_LENGTH * UNIT_TO_PIXEL;
    B.i = x_A;
    B.j = y_A;
    B.k = FOCAL_LENGTH * UNIT_TO_PIXEL;
    sigma_r = Vector.ang3( &A, &B );
    
    n_l = ZDIV( fabs( ( phi_A * ( the_B - the_A ) ) - ( the_A * ( phi_B - phi_A ) ) ), d__l );
    u_l = sqrt( A_l_sq - ( n_l * n_l ) );
    nu = atan2( ( n_l * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    up = atan2( ( u_l * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    
    /* Get proper device angles from kinetic */
    r.x = k->values.rotation[0];
    r.y = k->values.rotation[1];
    r.z = k->values.rotation[2];
    
    /* Generate Camera-Beacon quaternion */
    a.x = r.x + nu;
    a.y = r.y + psi_r;
    a.z = r.z - up;
    Quaternion.fromEuler( &a, k->qd );
    Quaternion.combine( k->qd, &k->qc, &k->qa );
    
    ang3Rad_To_Deg(&a);
    
    ///NOTE: Initializing another quaternion QD' cuts a quaternion combination
    a.x = r.x;
    /*  a.y should doens't change from the calculation of QD
     *  a.y = r.y + psi_r; */
    a.z = r.z;
    Quaternion.fromEuler( &a, &k->qd_ );
    ang3Rad_To_Deg(&a);
    
    /** CALCULATING R ***/
    double chi, mu, gam;
    double sin_sigma_r = sin( sigma_r );
    
    /* Calculate Chi */
    chi = RASIN( ZDIV( ZDIV( FOCAL_LENGTH, cos( sigma_A ) ), d__l ) * sin_sigma_r );
    
    /* Calculate mu */
    Quaternion.toMatrix(&k->qd_, &m);
    mu = -asin( m.jj );
    
    /* Calculate gamma */
    gam  = chi - mu;
    
    /* Calculate r vector length */
    r_l = ZDIV( sin( gam ), sin_sigma_r ) * D_FIXED;
    
    /* r_vec - Vector length r on X-axis */
    vec3_t r_u, r_f;
    r_u.i = 0;
    r_u.j = r_l;
    r_u.k = 0;
    
    Quaternion.toEuler(&k->qa, &a);
    a.z -= M_PI/2;
    Quaternion.fromEuler(&a, &k->qa);
    Quaternion.toEuler(&k->qa, &a);
    ang3Rad_To_Deg(&a);
    
    /*** FILTERING R VECTOR ***/
    /* Calculate final r vector */
    Quaternion.rotVec( &r_u, &k->qa, &r_f );
    
    k->values.position[0] = r_f.i;
    k->values.position[1] = r_f.j;
    k->values.position[2] = r_f.k;
    
    //    printf("Yaw:%4d | Nu:%4d | Up:%4d | Sig:%4d | Chi:%4d | Mu:%4d | Gamma:%4d | H_a: <%4d,%4d,%4d> | r_l: %.4f\n", (int)(r.z*RAD_TO_DEG), (int)(nu*RAD_TO_DEG), (int)(up*RAD_TO_DEG), (int)(sigma_r*RAD_TO_DEG), (int)(chi*RAD_TO_DEG), (int)(mu*RAD_TO_DEG), (int)(gam*RAD_TO_DEG), (int)(a.x), (int)(a.y), (int)(a.z), r_l);
    
    //    return;
    
    /* Kalman filter position */
    /* Get non-gravitational acceleration */
    vec3_t ngacc;
//    IMU_Get_Non_Grav( imu, &ngacc );
    double delta_time = 0;
    k->filters.position[0]->value = ngacc.i;
    k->filters.position[1]->value = ngacc.j;
    k->filters.position[2]->value = ngacc.k;
    
    /* Filter calculated r_vec with acceleration > velocity */
    float n;
    n = k->filters.position[0]->value;
    delta_time = seconds_since( k->filters.position[0]->timestamp );
    double x_vel = ngacc.i * delta_time + k->filters.position[0]->rate;
    Kalman.update( k->filters.position[0], r_f.i, x_vel );
    
    n = k->filters.position[1]->value;
    delta_time = seconds_since( k->filters.position[1]->timestamp );
    double y_vel = ngacc.j * delta_time + k->filters.position[1]->rate;
    Kalman.update( k->filters.position[1], r_f.j, y_vel );
    
    n = k->filters.position[2]->value;
    delta_time = seconds_since( k->filters.position[2]->timestamp );
    double z_vel = ngacc.k * delta_time + k->filters.position[2]->rate;
    Kalman.update( k->filters.position[2], r_f.k, z_vel );
    
    k->values.position[0] = k->filters.position[0]->value;
    k->values.position[1] = k->filters.position[1]->value;
    k->values.position[2] = k->filters.position[2]->value;
}

struct kinetic Kinetic =
{
    .init = init,
    .updateRotation = updateRotation,
    .updatePosition = updatePosition
};

/***********************************************************************************************//**
 *  \brief  Initialize Filters for Kinetic Data
 **************************************************************************************************/
//void Filters_Init( LSM9DS1_t * imu, kinetic_t * kinetics )
//{
//	IMU_Update_All( imu );
//    Kalman_Init( &k->filters.rotation[0], imu->data.roll, MOTION_MAX_KALMAN_LIFE, MOTION_VALUE_UNCERTAINTY, MOTION_BIAS_UNCERTAINTY, MOTION_SENSOR_UNCERTAINTY);
//    Kalman_Init( &k->filters.rotation[1], imu->data.pitch, MOTION_MAX_KALMAN_LIFE, MOTION_VALUE_UNCERTAINTY, MOTION_BIAS_UNCERTAINTY, MOTION_SENSOR_UNCERTAINTY);
//    Kalman_Init( &k->filters.rotation[2], imu->data.yaw, MOTION_MAX_KALMAN_LIFE, MOTION_VALUE_UNCERTAINTY, MOTION_BIAS_UNCERTAINTY, MOTION_SENSOR_UNCERTAINTY);
//}

void Camera_Rotation_Init( kinetic_t * k )
{
    ang3_t c_a;
    c_a.x = CAMERA_OFFSET_ANGLE_X;
    c_a.y = CAMERA_OFFSET_ANGLE_Y;
    c_a.z = CAMERA_OFFSET_ANGLE_Z;
    Quaternion.fromEuler( &c_a, &k->qc );
}

