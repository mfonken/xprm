//
//  main.cpp
//  camerarollzoomtest
//
//  Created by Matthew Fonken on 11/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include <math.h>

#define DEG(X) (X/M_PI * 180.)

using namespace std;

typedef struct
{
    double i, j;
} vec2_t;

typedef struct
{
    double x,y;
} point2_t;

static double dot(vec2_t* a, vec2_t* b)
{
    return a->i*b->i + a->j*b->j;
}

static double len(vec2_t* a)
{
    return sqrt(a->i*a->i + a->j*a->j);
}
static vec2_t add(vec2_t* a, vec2_t* b)
{
    return (vec2_t){ a->i + b->i, a->j + b->j };
}
static vec2_t sub(vec2_t* a, vec2_t* b)
{
    return (vec2_t){ a->i - b->i, a->j - b->j };
}

static bool intersection( point2_t* a, point2_t* A, point2_t* b, point2_t* B, point2_t* P)
{
    printf("Finding intersection of line (%.2f, %.2f)-(%.2f, %.2f) and (%.2f, %.2f)-(%.2f, %.2f): ", a->x, a->y, A->x, A->y, b->x, b->y, B->x, B->y);
    
    vec2_t ab = sub( (vec2_t*)a,  (vec2_t*)b );
    vec2_t bB = sub( (vec2_t*)b,  (vec2_t*)B );
    vec2_t aA = sub( (vec2_t*)a,  (vec2_t*)A );
    
    double num = ab.i * bB.j - ab.j * bB.i;
    double det = aA.i * bB.j - aA.j * bB.i;
    if( num == 0 || det == 0 )
    {
        printf("None\n");
        return false;
    }
    double t = num / det;
    
    P->x = a->x + t * - aA.i;
    P->y = a->y + t * - aA.j;
    
    printf("(%.2f, %.2f)\n", P->x, P->y);
    
    return true;
}

static vec2_t rot(vec2_t* a, double theta)
{
    double cos_theta = cos(theta), sin_theta = sin(theta);
    return (vec2_t){ a->i * cos_theta - a->j * sin_theta, a->i * sin_theta + a->j * cos_theta };
}

static double angle(vec2_t* a, vec2_t* b)
{
    double num = dot(a, b);
    double det = len(a) * len(b);
    if(  num == 0 || det == 0 ) return M_PI_2;
    return acos( num / det );
}

static double delta_R(double r, vec2_t* ab, vec2_t* AB )
{
    double d = len( ab ), D = len( AB );
    return r * ( 1 - D / d );
}

int main(int argc, const char * argv[])
{
    double f = 5;
    vec2_t a = { 0., 0. }, b = { 2., 2. };
    vec2_t A = { -2., 2 }, B = { 2., -2. };
    
    point2_t P;
    intersection((point2_t*)&a, (point2_t*)&b, (point2_t*)&A, (point2_t*)&B, &P);

    vec2_t ab = sub(&b, &a);
    vec2_t AB = sub(&B, &A);
    
//    double theta = atan2( ab.j, ab.i );
    
    double sigma = angle( &ab, &AB );
    
    double sigma_deg = DEG(sigma);
    
    double r = 1.;
    double dR = delta_R( r, &ab, &AB );
//    double R = r + dR;
    
    vec2_t daA = sub(&A, &a);
    vec2_t dbB = sub(&B, &b);
    vec2_t aAbB = add( &daA, &dbB );
    vec2_t simga_aAbB = rot( &aAbB, sigma );// - theta );
    
    double phi = atan2( -simga_aAbB.i, f ),  psi = atan2( simga_aAbB.j, f );
    double phi_deg = DEG(phi), psi_deg = DEG(psi);
    printf("(%.2f, %.2f, %.2f)º | ∆R=%.2f\n", sigma_deg, phi_deg, psi_deg, dR);
    
    return 0;
}
