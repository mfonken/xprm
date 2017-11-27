
//  cr.cpp
//  crtest
//
//  Created by Matthew Fonken on 11/17/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "cr.hpp"

#define TSM

#define ZDIV_LNUM 1 << 10
#define ZDIV(X,Y) Y==0?ZDIV_LNUM:X/Y
#define RASIN(X)  X>=1?M_PI/2:asin(X)

quaternion_t qd, qr, qc, qc_, qd_, qg, qa;

void ang3Rad_To_Deg( ang3_t * a )
{
    a->x *= RAD_TO_DEG;
    a->y *= RAD_TO_DEG;
    a->z *= RAD_TO_DEG;
}

void ang3RDeg_To_Rad( ang3_t * a )
{
    a->x *= DEG_TO_RAD;
    a->y *= DEG_TO_RAD;
    a->z *= DEG_TO_RAD;
}

/* Quaternion Combination Test */
void qCT()
{
    quaternion_t qa, qb, qc;
//    ang3_t aa = {-45,0,45}, ab = {0, -21.47067664,21.47067664}, ac;
    ang3_t aa = {-45,0,45}, ab = {0, 20, 0}, ac;
    ang3RDeg_To_Rad(&aa);
    ang3RDeg_To_Rad(&ab);
    Euler_To_Quaternion(&aa, &qa);
    Euler_To_Quaternion(&ab, &qb);

    Quaternion_Combine(&qa, &qb, &qc);
    Quaternion_To_Euler(&qc, &ac);
    Euler_To_Quaternion(&aa, &qb);
    aa.y += ab.y;
    Euler_To_Quaternion(&aa, &qa);
    
    ang3Rad_To_Deg(&aa);
    ang3Rad_To_Deg(&ab);
    ang3Rad_To_Deg(&ac);
    
    vec3_t va = {10,0,0}, vb = {0,10,0}, vc = {0,0,10}, ra, rb, rc;
    Rotate_Vector_By_Quaternion(&va, &qc, &ra);
    Rotate_Vector_By_Quaternion(&vb, &qc, &rb);
    Rotate_Vector_By_Quaternion(&vc, &qc, &rc);
}

void cr( kinetic_t * kinetics, cartesian2_t beacons[2] )
{
    mat3x3_t m;
    ang3_t a;
    
    /* Tait-Bryan (intrinsic) angles of imu */
    ang3_t tbd, tbr;
    /* IMU -> Kinetic axis correction:
     *  - Rotate Yaw by -90º
     *  - Invert Yaw */
    
    double the_1, the_2, phi_1, phi_2, psi_r, sigma_1, sigma_2, sigma_r;
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
    
//    tbd.x = kinetics->rotation[0]; // phi'
//    tbd.y = kinetics->rotation[1]; // theta'
//    tbd.z = /*M_PI / 2 - */-kinetics->rotation[2]; // psi'
//    Euler_To_Quaternion( &tbd, &qd );
    
    /* Get angles between beacons */
    //    the_r = the_1 - the_2;
    //    phi_r = phi_1 - phi_2;
    sigma_1 = acos( cos( the_1 ) * cos( phi_1 ) );
    sigma_2 = acos( cos( the_2 ) * cos( phi_2 ) );
    sigma_r = sigma_1 + sigma_2;
    
    tbd.x = kinetics->rotation[0]; // phi'
    tbd.y = kinetics->rotation[1] + psi_r; // theta'
    tbd.z = /*M_PI / 2 - */-kinetics->rotation[2]; // psi'
    Euler_To_Quaternion( &tbd, &qd );
    
    Quaternion_To_Matrix(&qd, &m);
    Quaternion_To_Euler(&qd, &a);
    a.x *= RAD_TO_DEG;
    a.y *= RAD_TO_DEG;
    a.z *= RAD_TO_DEG;
    
    /* Get distance between beacons */
    d__l = FOCAL_LENGTH * ( tan( sigma_1 ) + tan( sigma_2 ) );//sqrt( ( d_x * d_x ) + ( d_y * d_y ) ) * PIXEL_TO_UNIT;
    
#ifdef TSM
    /*** Tangent Surface Mapping Test ***/
    double x_1_n, x_2_n, y_1_n, y_2_n;
    double a_1, a_2;
    double x_1_sq, x_2_sq, y_1_sq, y_2_sq, l_1, l_2, t_1, t_2, s_1, s_2, A, B, C, D, E, F, G, D__l;
//    x_1_n = x_1 - 640;
//    x_2_n = x_2 - 640;
//    y_1_n = y_1 - 400;
//    y_2_n = y_2 - 400;
    x_1_n = the_1;
    x_2_n = the_2;
    y_1_n = phi_1;
    y_2_n = phi_2;
    
    x_1_sq = x_1_n * x_1_n;
    x_2_sq = x_2_n * x_2_n;
    y_1_sq = y_1_n * y_1_n;
    y_2_sq = y_2_n * y_2_n;
    l_1 = sqrt( x_1_sq + y_1_sq ); // = sigma_1
    l_2 = sqrt( x_2_sq + y_2_sq ); // = sigma_2
    t_1 = tan( l_1 );
    t_2 = tan( l_2 );
    
    a_1 = atan2(y_1_n, x_1_n);
    a_2 = atan2(y_2_n, x_2_n);
    
    A = t_2 * cos(a_2);
    B = t_1 * cos(a_1);
    C = t_2 * sin(a_2);
    D = t_1 * sin(a_1);
    
//
//    s_1 = sqrt( ( y_1_sq / x_1_sq ) + 1 );
//    s_2 = sqrt( ( y_2_sq / x_2_sq ) + 1 );
//    
//    A = t_2 / s_2;
//    B = t_1 / s_1;
//    C = ( y_2_n / x_2_n ) * A;
//    D = ( y_1_n / x_1_n ) * B;
//
    
    E = ( A - B );
    F = ( C - D );
    G = sqrt( E*E + F*F );
    D__l = FOCAL_LENGTH * G;
    
//    printf("d__l: %f vs D__l: %f > %f\n", d__l, D__l, d__l/D__l);
    
    d__l = D__l;
    /************************************/
#endif
    
    double chi, mu, gam;
    double sin_sigma_r = sin( sigma_r );
    
//    double t = 2*tan(sigma_1), d__f = d__l / t;
    
    /* Calculate Chi */
//    double TEMP_FOCAL_ADJ = 1.1;
//    double da, db, dc, dd;
//    da = FOCAL_LENGTH;
//    db =
//    dc = ZDIV( ZDIV( FOCAL_LENGTH, cos( sigma_1 ) ), d__l ) * sin_sigma_r;
//    dd = RASIN(dc);
    chi = RASIN( ZDIV( ZDIV( FOCAL_LENGTH, cos( sigma_1 ) ), d__l ) * sin_sigma_r );
    Quaternion_Combine( &qd, &qc, &qd_ );
    Quaternion_To_Euler(&qd_, &a);
    a.x *= RAD_TO_DEG;
    a.y *= RAD_TO_DEG;
    a.z *= RAD_TO_DEG;
    
    /* Calculate mu */
    Quaternion_To_Matrix(&qd_, &m);
    mu = -asin( m.jj );
    
    /* Calculate gamma */
    gam  = chi - mu;
    
    /* Calculate r vector length */
    r_l = ZDIV( sin( gam ), sin_sigma_r ) * D_FIXED;
    
    /* r_vec - Vector length r on X-axis */
    vec3_t r, r_f;
    r.i = r_l;
    r.j = 0;
    r.k = 0;

    double ups;
    ups = fabs( ( phi_1 * ( the_2 - the_1 ) ) - ( the_1 * ( phi_2 - phi_1 ) ) ) / d__l;
    
    /* Generate Camera-Beacon quaternion */
    /* Tau -> Kinetic axis correction */
    tbr.x = 0;//phi_1;
    tbr.y = 0;//psi_r;
    tbr.z = -sigma_1;//the_1
    Euler_To_Quaternion( &tbr, &qr );
    tbr.x *= RAD_TO_DEG;
    tbr.y *= RAD_TO_DEG;
    tbr.z *= RAD_TO_DEG;
    
    ups *= RAD_TO_DEG;
    
    Quaternion_Combine( &qd_, &qr, &qa );

    Quaternion_To_Euler(&qd_, &a);
    a.x *= RAD_TO_DEG;
    a.y *= RAD_TO_DEG;
    a.z *= RAD_TO_DEG;
    Quaternion_To_Euler(&qr, &a);
    a.x *= RAD_TO_DEG;
    a.y *= RAD_TO_DEG;
    a.z *= RAD_TO_DEG;
    Quaternion_To_Euler(&qa, &a);
    a.x *= RAD_TO_DEG;
    a.y *= RAD_TO_DEG;
    a.z *= RAD_TO_DEG;

    /* Calculate final r vector */
    Rotate_Vector_By_Quaternion( &r, &qa, &r_f );
    kinetics->position[0] = r_f.i;
    kinetics->position[1] = r_f.j;
    kinetics->position[2] = r_f.k;
    
//    printf("\tChi: %4d | Mu: %4d | Gamma: %4d | H_a: <%4d, %4d, %4d> | r_l: %.4f\n", (int)(chi*RAD_TO_DEG), (int)(mu*RAD_TO_DEG), (int)(gam*RAD_TO_DEG), (int)(a.x), (int)(a.y), (int)(a.z), r_l);
}

void Camera_Rotation_Init( void )
{
    ang3_t c_a;
    c_a.x = CAMERA_OFFSET_ANGLE_X;
    c_a.y = CAMERA_OFFSET_ANGLE_Y;
    c_a.z = CAMERA_OFFSET_ANGLE_Z;
    Euler_To_Quaternion( &c_a, &qc );
}
