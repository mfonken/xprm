#include <stdio.h>
#include <string>
#include "../utilities/i2c_sp.c"
#include "../sensors/LSM9DS1.c"
#include "../kinetic/kinetic.c"

LSM9DS1_t imu;

void setup( std::string * msg )
{
    printf("Starting Mag Calibration Test...\n");
    printf("Initing I2C...\n");
    I2C_Init();
    
    printf("Initing IMU.\n");
    IMU_Init(&imu);

    printf("Validating IMU: ");
    if( IMU_Verify(&imu) ) printf("Success.\n");
    else
    {
	printf("Failure.\n");
	return;
    }
    
}

void loop( std::string * msg )
{
    IMU_Update_Mag( &imu );
    char buff[48];
    sprintf(buff, "m, %f, %f, %f,\r\n", imu.data.mag[0], imu.data.mag[1], imu.data.mag[2] );
    (*msg).assign(buff);
}

