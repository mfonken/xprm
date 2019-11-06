//
//  main.cpp
//  kineticerrortest
//
//  Created by Matthew Fonken on 11/2/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>

#include <Eigen/Dense>
#include <random>
#include <fstream>

#include "test_config.h"
#include "kinetic_master.h"

using namespace std;

struct normal_random_variable
{
    normal_random_variable(Eigen::MatrixXd const& covar)
        : normal_random_variable(Eigen::VectorXd::Zero(covar.rows()), covar)
    {}

    normal_random_variable(Eigen::VectorXd const& mean, Eigen::MatrixXd const& covar)
        : mean(mean)
    {
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(covar);
        transform = eigenSolver.eigenvectors() * eigenSolver.eigenvalues().cwiseSqrt().asDiagonal();
    }

    Eigen::VectorXd mean;
    Eigen::MatrixXd transform;

    Eigen::VectorXd operator()() const
    {
        static std::mt19937 gen{ std::random_device{}() };
        static std::normal_distribution<> dist;

        return mean + transform * Eigen::VectorXd{ mean.size() }.unaryExpr([&](auto x) { return dist(gen); });
    }
};

typedef struct normal_random_variable normal_random_variable;
static int ln = 0;
static void print(kpoint_t& A, kpoint_t& B, ang3_t& O, vec3_t R)
{
    printf("A:(%6.3f, %6.3f, %6.3f)px | B:(%6.3f, %6.3f, %6.3f)px | O:(%6.3f, %6.3f, %6.3f)º > R:(%7.3f, %7.3f, %7.3f)m\n", A.x, A.y, A.z, B.x, B.y, B.z, O.x, O.y, O.z, R.i, R.j, R.k);
    
    ofstream file(OUT_FILE_NAME, ofstream::out | ofstream::app);
    file << ++ln << "," << A.x << "," << A.y << "," << A.z << "," << B.x << "," << B.y << "," << B.z << "," << O.x << "," << O.y << "," << O.z << "," << R.i * 1000 << "," << R.j * 1000 << "," << R.k * 1000 << endl;
    file.close();

}

static void run(kinetic_t * kin, int cycles, Eigen::Vector2d A_new, Eigen::Vector2d B_new, Eigen::Vector3d O_new )
{
    Eigen::MatrixXd P_covar(2,2);
    P_covar << P_COVAR,0,0,P_COVAR; //px
    normal_random_variable A_norm( A_new, P_covar );
    normal_random_variable B_norm( B_new, P_covar );
    
    unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    double O_covar = O_COVAR * DEG_TO_RAD;
    std::normal_distribution<double> O_x_norm(O_new[0], O_covar);
    std::normal_distribution<double> O_y_norm(O_new[1], O_covar);
    std::normal_distribution<double> O_z_norm(O_new[2], O_covar);

    ang3_t g = {0,0,0}; // gyro
    vec3_t n = {0,0,1}; // non-gravity

    kpoint_t A_ = {0}, B_ = {0};
    ang3_t O_ = {0};
    
    print(A_, B_, O_, {0,0,0});
    print(A_, B_, O_, {0,0.025,0});
    print(A_, B_, O_, {0,0.1,0});
    print(A_, B_, O_, {0.08,0.1,0});
    
    for(uint16_t i = 0; i < cycles; i++)
    {
        O_ = (ang3_t){ O_x_norm(generator), O_y_norm(generator), O_z_norm(generator) };
        
        KineticFunctions.UpdateRotation( kin, &O_, &g );
        
        Eigen::Vector2d A = A_norm();
        Eigen::Vector2d B = B_norm();
        
        A_ = (kpoint_t){ A[0], A[1] };
        B_ = (kpoint_t){ B[0], B[1] };
        
        KineticFunctions.UpdatePosition( kin, &n, A_, B_ );
        
        print(A_, B_, O_, kin->r);
        
#ifdef __KALMAN__
        if(isnan(kin->filters.rotation[0].value))
           printf("!");
        if(isnan(kin->filters.position[0].value))
            printf("¡");
    }
    printf("Rotations:\n");
    for(uint8_t i = 0; i < 3; i++)
        Kalman.Print(&kin->filters.rotation[i]);
    printf("Positions:\n");
    for(uint8_t i = 0; i < 3; i++)
        Kalman.Print(&kin->filters.position[i]);
#else
    }
#endif
}

int main(int argc, const char * argv[])
{
    ofstream file(OUT_FILE_NAME);
    if(!file.is_open())
    {
        printf("Failed to open %s.", OUT_FILE_NAME);
        return 1;
    }
    file << "#,Ax,Ay,Az,Bx,By,Bz,Ox,Oy,Oz,Rx,Ry,Rz\n";
    file.close();
    
    kinetic_t kin;
    KineticFunctions.DefaultInit( &kin );

    Eigen::Vector2d A,B;
    A[0] = P_X_MIN;
    A[1] = P_Y_MIN;
    
    Eigen::Vector3d O;
    O[0] = 45 * DEG_TO_RAD;
    O[1] = 10 * DEG_TO_RAD;
    O[2] = -M_PI_2;
    
    B[0] = P_X_MAX; B[1] = P_Y_MIN;
    
    run(&kin, CYCLES, A, B, O);
    
    return 0;
}
