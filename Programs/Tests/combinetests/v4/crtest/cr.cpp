
//  cr.cpp
//  crtest
//
//  Created by Matthew Fonken on 11/17/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "cr.hpp"

quaternion_t qd, qd_, qs, qs_, qc, qc_, qr, qg, qa;

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
    
    /* Local angles */
    ang3_t a;
    double the_1, the_2, phi_1, phi_2, psi_r, sigma_1, sigma_r, nu, up;
    
    /* Local lengths */
    double x_1, x_2, y_1, y_2, d_x, d_y, d__1_sq, d__l, d__n, d__u, r_l;
    
    x_1 = beacons[0].x - CAMERA_HALF_WIDTH;
    x_2 = beacons[1].x - CAMERA_HALF_WIDTH;
    y_1 = beacons[0].y - CAMERA_HALF_HEIGHT;
    y_2 = beacons[1].y - CAMERA_HALF_HEIGHT;
    d_x = x_2 - x_1;
    d_y = y_2 - y_1;
    
    /* Get pixel distance from (0,0) to beacon 1 */
    d__1_sq = ( ( x_1 * x_1 ) + ( y_1 * y_1 ) );
    
    /* Get unit distance between beacons */
    d__l = sqrt( ( d_x * d_x ) + ( d_y * d_y ) ) * PIXEL_TO_UNIT;
    
/*** CALCULATING QUATERNIONS START ***/
    /* Get beacon angles */
    the_1   = atan2( ( x_1 * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    phi_1   = atan2( ( y_1 * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    the_2   = atan2( ( x_2 * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    phi_2   = atan2( ( y_2 * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    psi_r   = atan2( d_y, d_x );
    
    /* Get angles between beacons */
    ///NOTE: acos(cos()cos()) only works for combining frame-perpendicular vectors (i.e. theta & phi) [ang3 is used instead for sigma_r]
    sigma_1 = acos( cos( the_1 ) * cos( phi_1 ) );

    vec3_t A, B;
    A.i = x_1;
    A.j = y_1;
    A.k = FOCAL_LENGTH * UNIT_TO_PIXEL;
    B.i = x_2;
    B.j = y_2;
    B.k = FOCAL_LENGTH * UNIT_TO_PIXEL;
    sigma_r = ang3( &A, &B );

    d__n = ZDIV( fabs( ( phi_1 * ( the_2 - the_1 ) ) - ( the_1 * ( phi_2 - phi_1 ) ) ), d__l );
    d__u = sqrt( d__1_sq - ( d__n * d__n ) );
    nu = atan2( ( d__n * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    up = atan2( ( d__u * PIXEL_TO_UNIT ), FOCAL_LENGTH);
    
    /* Generate Camera-Beacon quaternion */
    a.x = kinetics->rotation[0] + nu;
    a.y = kinetics->rotation[1] + psi_r;
    a.z = kinetics->rotation[2] - up;
    Euler_To_Quaternion( &a, &qd );
    Quaternion_Combine( &qd, &qc, &qa );
    
    ///NOTE: Initializing another quaternion QD' cuts a quaternion combination
    a.x = kinetics->rotation[0];
/*  a.y should doens't change from the calculation of QD
 *  a.y = kinetics->rotation[1] + psi_r; */
    a.z = kinetics->rotation[2];
    Euler_To_Quaternion( &a, &qd_ );
/*** CALCULATING QUATERNIONS END ***/
    
/** CALCULATING R START ***/
    double chi, mu, gam;
    double sin_sigma_r = sin( sigma_r );
    
    /* Calculate Chi */
    chi = RASIN( ZDIV( ZDIV( FOCAL_LENGTH, cos( sigma_1 ) ), d__l ) * sin_sigma_r );
    
    /* Calculate mu */
    Quaternion_To_Matrix(&qd_, &m);
    mu = -asin( m.jj );
    
    /* Calculate gamma */
    gam  = chi - mu;
    
    /* Calculate r vector length */
    r_l = ZDIV( sin( gam ), sin_sigma_r ) * D_FIXED;
    
    /* r_vec - Vector length r on X-axis */
    vec3_t r, r_f;
    r.i = 0;
    r.j = r_l;
    r.k = 0;

    Quaternion_To_Euler(&qa, &a);
    a.z -= M_PI/2;
    Euler_To_Quaternion(&a, &qa);
    a.x *= RAD_TO_DEG;
    a.y *= RAD_TO_DEG;
    a.z *= RAD_TO_DEG;
    
    /* Calculate final r vector */
    Rotate_Vector_By_Quaternion( &r, &qa, &r_f );
    kinetics->position[0] = r_f.i;
    kinetics->position[1] = r_f.j;
    kinetics->position[2] = r_f.k;
/** CALCULATING R END ***/
 
#ifndef PRINT_POS
    printf("\tNu:%4d | Up:%4d | Sig:%4d | Chi:%4d | Mu:%4d | Gamma:%4d | H_a: <%4d,%4d,%4d> | r_l: %.4f\n", (int)(nu*RAD_TO_DEG), (int)(up*RAD_TO_DEG), (int)(sigma_r*RAD_TO_DEG), (int)(chi*RAD_TO_DEG), (int)(mu*RAD_TO_DEG), (int)(gam*RAD_TO_DEG), (int)(a.x), (int)(a.y), (int)(a.z), r_l);
#endif
}

void Camera_Rotation_Init( void )
{
    ang3_t c_a;
    c_a.x = CAMERA_OFFSET_ANGLE_X;
    c_a.y = CAMERA_OFFSET_ANGLE_Y;
    c_a.z = CAMERA_OFFSET_ANGLE_Z;
    Euler_To_Quaternion( &c_a, &qc );
}
