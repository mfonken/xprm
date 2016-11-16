//
//  imu.h
//  
//
//  Created by Matthew Fonken on 11/15/16.
//
//

#ifndef imu_h
#define imu_h

/*! IMU even handler */
void imuEvent()
{
    getIMU();
    
    uint32_t theta = getPitch();
    uint32_t phi   = getRoll();
    uint32_t psi   = getYaw();
    
    uint32_t kalman_t_temp = rot_f[0].value;
    if ((theta < -HALF_PI && kalman_t_temp > HALF_PI) || (theta > HALF_PI && kalman_t_temp < -HALF_PI))
    {
        rot_f[1].value  = theta;
        rot[1]          = theta;
    }
    else
    {
        updatekalman_t( &rot_f[1], theta, gyro[1], delta_t ); // Calculate the true pitch using a kalman_t filter
        rot[1] = rot_f[1].value;
    }
    
    kalman_t_temp = absl(rot_f[1].value);
    if (kalman_t_temp > HALF_PI)
    {
        gyro[0] = -gyro[0]; // Invert rate, so it fits the restriced accelerometer reading
    }
    updatekalman_t( &rot_f[0], phi, gyro[0], delta_t ); // Calculate the true roll using a kalman_t filter
    rot[0] = rot_f[0].value;
    
    updatekalman_t( &rot_f[2], psi, gyro[2], delta_t ); // Calculate the true yaw using a kalman_t filter

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
vec3_t * getNonGravAcceleration()
{
    /* Tait-Bryan angles of vision */
    ang3_t tba;
    tba.a = rot_f[0].value;
    tba.b = rot_f[1].value;
    tba.c = rot_f[2].value;
    
    /* Create a vector of accelerometer values */
    vec3_t avec;
    avec.ihat = acc[0];
    avec.jhat = acc[1];
    avec.khat = acc[2];
    
    /* Transform and normalize v vector by given angles to get unit vector from camera */
    vec3_t * atru = zxyTransform( &avec, &tba, 1 );
    
    atru->khat += 1; // Negate gravity
    return atru;
}

#endif /* imu_h */
