//
//  cr.cpp
//  crtest
//
//  Created by Matthew Fonken on 11/17/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "cr.hpp"

#define ZDIV_LNUM 1 << 10
#define ZDIV(X,Y) Y==0?ZDIV_LNUM:X/Y

quaternion_t qd, qr, qc, qd_, qg, qa;

void cr( kinetic_t * kinetics, cartesian2_t beacons[2] )
{
    mat3x3_t m;
    ang3_t a;
    
    /* Tait-Bryan (intrinsic) angles of imu */
    ang3_t tbd, tbr;
    tbd.x = kinetics->rotation[0]; // phi'
    tbd.y = kinetics->rotation[1]; // theta'
    tbd.z = kinetics->rotation[2]; // psi'
    Euler_To_Quaternion( &tbd, &qd );
    
    double the_1, the_2, the_r, phi_1, phi_2, phi_r, psi_r, rho_1, rho_r;
    double x_1, x_2, y_1, y_2, d_x, d_y, d__l, r_l;
    
    x_1 = beacons[0].x;
    x_2 = beacons[1].x;
    y_1 = beacons[0].y;
    y_2 = beacons[1].y;
    d_x = x_2 - x_1;
    d_y = y_2 - y_1;
    
    /* Get beacon angles */
    the_1   = CAMERA_ALPHA_W * DEG_TO_RAD * ( ( x_1 / CAMERA_WIDTH  ) - 0.5 );
    phi_1   = CAMERA_ALPHA_H * DEG_TO_RAD * ( ( y_1 / CAMERA_HEIGHT ) - 0.5 );
    the_2   = CAMERA_ALPHA_W * DEG_TO_RAD * ( ( x_2 / CAMERA_WIDTH  ) - 0.5 );
    phi_2   = CAMERA_ALPHA_H * DEG_TO_RAD * ( ( y_2 / CAMERA_HEIGHT ) - 0.5 );
    psi_r   = atan2( d_y, d_x );
    
    /* Get angles between beacons */
    the_r = fabs( the_2 - the_1 );
    phi_r = fabs( phi_2 - phi_1 );
    rho_1 = acos( cos( the_1 ) * cos( phi_1 ) );
    rho_r = acos( cos( the_r ) * cos( phi_r ) );
    
    /* Get distance between beacons */
    d__l = sqrt( ( d_x * d_x ) + ( d_y * d_y ) ) * PIXEL_TO_METER;
    
    double chi, mu, gam;
    double sin_rho_r = sin( rho_r );
    
    /* Calculate Chi */
    double f__l, f__d, pre_chi, f = /* FOCAL_LENGTH */ d__l * cos(rho_1);
    f__l = ZDIV( f, cos( rho_1 ) );
    f__d = ZDIV( f__l, d__l );
    pre_chi = f__d * sin_rho_r;
    chi = asin( pre_chi );

//    chi = asin( ZDIV( ( /* FOCAL_LENGTH */ d__l ), d__l ) * ZDIV( sin_rho_r, cos( rho_1 ) ) );
//    chi = asin( ZDIV( ZDIV( /* FOCAL_LENGTH */ d__l, cos( rho_1 ) ), d__l ) * sin_rho_r );
    
    Quaternion_Combine( &qd, &qc, &qd_ );
    Quaternion_To_Euler(&qd_, &a);

    
    /* Calculate mu */
//    mu = asin( ( qr.w * qr.w ) - ( qr.x * qr.x ) - ( qr.y * qr.y ) + ( qr.z * qr.z ) );
    Quaternion_To_Matrix(&qd_, &m);
    mu = asin( m.ii );
    
    /* Calculate gamma */
    gam  = chi - mu;
//    printf("\tChi: %4d | Mu: %4d | Gamma: %4d | D': <%4d, %4d, %4d>\n", (int)(chi*RAD_TO_DEG), (int)(mu*RAD_TO_DEG), (int)(gam*RAD_TO_DEG), (int)(a.x*RAD_TO_DEG), (int)(a.y*RAD_TO_DEG), (int)(a.z*RAD_TO_DEG));
    
    /* Calculate r vector length */
    r_l = ( sin( gam ) / sin_rho_r ) * D_FIXED;
    
    /* r_vec - Vector length r on X-axis */
    vec3_t r, r_f;
    r.i = r_l;
    r.j = 0;
    r.k = 0;
    
//    printf("||r||: %.4f (cm)\n", r_l * 100);
    
    /* Generate Camera-Beacon quaternion */
    tbr.y = phi_1;
    tbr.z = 270*DEG_TO_RAD - the_1;
    tbr.x = psi_r;
    Euler_To_Quaternion( &tbr, &qr );
    
    Quaternion_Combine( &qd_, &qr, &qa );

    Quaternion_To_Euler(&qd_, &a);
//    printf("D': <%4d, %4d, %4d> | ", (int)(a.x*RAD_TO_DEG), (int)(a.y*RAD_TO_DEG), (int)(a.z*RAD_TO_DEG));
    Quaternion_To_Euler(&qr, &a);
//    printf("R: <%4d, %4d, %4d> | ", (int)(a.x*RAD_TO_DEG), (int)(a.y*RAD_TO_DEG), (int)(a.z*RAD_TO_DEG));
    Quaternion_To_Euler(&qa, &a);
//    printf("A: <%4dº, %4dº, %4dº> | ", (int)(a.x*RAD_TO_DEG), (int)(a.y*RAD_TO_DEG), (int)(a.z*RAD_TO_DEG));
    
    /* Calculate final r vector */
    Rotate_Vector_By_Quaternion( &r, &qa, &r_f );
    kinetics->position[0] = r_f.i;
    kinetics->position[1] = r_f.j;
    kinetics->position[2] = r_f.k;
//    printf("r: (%.4f, %.4f, %.4f)\n", r_f.i, r_f.j, r_f.k);
}

void Camera_Rotation_Init( void )
{
    ang3_t c_a;
    c_a.x = CAMERA_OFFSET_ANGLE_X;
    c_a.y = CAMERA_OFFSET_ANGLE_Y;
    c_a.z = CAMERA_OFFSET_ANGLE_Z;
    Euler_To_Quaternion( &c_a, &qc );
}
