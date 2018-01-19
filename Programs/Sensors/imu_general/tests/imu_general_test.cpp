#include <stdio.h>
#include <string>
#include "../utilities/i2c_sp.c"
#include "../sensors/LSM9DS1.c"
#include "../kinetic/kinetic.c"

cartesian2_t bns[2];
LSM9DS1_t imu;
kinetic_t kin;

void setup( std::string * msg )
{
    printf("Starting...\n");
    printf("Initing I2C...\n");
    I2C_Init();
    
    printf("Initing beacons.\n");    
    bns[0].x = 400;
    bns[0].y = 100;
    bns[1].x = 100;
    bns[1].y = 400;
    
    printf("Initing IMU.\n");
    IMU_Init(&imu);

    printf("Validating IMU: ");
    if( IMU_Verify(&imu) ) printf("Success.\n");
    else
    {
	printf("Failure.\n");
	return;
    }
    
    printf("Initing kinetic.\n");
    Kinetic_Init( &imu, &kin );
}

void loop( std::string * msg )
{
    IMU_Update_All( &imu );
    char buff[100];
    sprintf(buff, "r, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f,\n", -imu.data.accel[0], -imu.data.accel[1], -imu.data.accel[2], -imu.data.gyro[0], -imu.data.gyro[1], -imu.data.gyro[2], -imu.data.mag[0], -imu.data.mag[1], -imu.data.mag[2] );
    (*msg).assign(buff);
}

