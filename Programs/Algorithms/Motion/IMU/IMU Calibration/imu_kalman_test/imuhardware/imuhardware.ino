#include <Wire.h>
#include <SPI.h>

#include <kinetic.h>
#include "MPU9250.h"
#include "quaternionFilters.h"
#define AHRS true         // Set to false for basic data read
#define SerialDebug false  // Set to true to get Serial output for debugging

#define SRV       9

#define BFR_SIZE  15
float sampleBuffer[BFR_SIZE][9];
int bufferIndex = 0;
float bufferSum[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

// Pin definitions
int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int myLed  = 13;  // Set up pin 13 led for toggling

MPU9250 myIMU;
Kinetic kinetic;

//float mb[3] = {0,0,0};
float mb[3] = {190, 260, -497};

float correction[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup()
{
  Serial.begin(9600);
  //Serial.setTimeout(10);
  //kinetic.begin();
  Wire.begin();

  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print("MPU9250 "); Serial.print("I AM "); Serial.print(c, HEX);
  Serial.print(" I should be "); Serial.println(0x71, HEX);

  if (c == 0x71) // WHO_AM_I should always be 0x68
  {
    Serial.println("MPU9250 is online...");

    // Start by performing self test and reporting values
    myIMU.MPU9250SelfTest(myIMU.SelfTest);
    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);

    myIMU.initMPU9250();
    Serial.println("MPU9250 initialized.");

    byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    Serial.print("AK8963 "); Serial.print("I AM "); Serial.print(d, HEX);
    Serial.print(" I should be "); Serial.println(0x48, HEX);
    myIMU.initAK8963(myIMU.magCalibration);
    Serial.println("AK8963 initialized.");
    if (SerialDebug)
    {
      //  Serial.println("Calibration values: ");
      Serial.print("X-Axis sensitivity adjustment value ");
      Serial.println(myIMU.magCalibration[0], 2);
      Serial.print("Y-Axis sensitivity adjustment value ");
      Serial.println(myIMU.magCalibration[1], 2);
      Serial.print("Z-Axis sensitivity adjustment value ");
      Serial.println(myIMU.magCalibration[2], 2);
    }
  }
  else
  {
    Serial.print("Could not connect to MPU9250: 0x");
    Serial.println(c, HEX);
    while (1) ; // Loop forever if communication doesn't happen
  }
}

void loop()
{
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
    myIMU.getAres();
    myIMU.ax = (float)myIMU.accelCount[0] * myIMU.aRes - correction[0]; // - accelBias[0];
    myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes - correction[1]; // - accelBias[1];
    myIMU.az = (float)myIMU.accelCount[2] * myIMU.aRes - correction[2]; // - accelBias[2];

    /* Normailize */
    double an = sqrt(myIMU.ax * myIMU.ax + myIMU.ay * myIMU.ay + myIMU.az * myIMU.az);
    myIMU.ax /= an;
    myIMU.ay /= an;
    myIMU.az /= an;

    myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values
    myIMU.getGres();
    myIMU.gx = (float)myIMU.gyroCount[0] * myIMU.gRes - correction[3];
    myIMU.gy = (float)myIMU.gyroCount[1] * myIMU.gRes - correction[4];
    myIMU.gz = (float)myIMU.gyroCount[2] * myIMU.gRes - correction[5];

    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
    myIMU.getMres();
    myIMU.magbias[0] = mb[0];
    myIMU.magbias[1] = mb[1];
    myIMU.magbias[2] = mb[2];
    
    myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes * myIMU.magCalibration[0] -
               myIMU.magbias[0] - correction[6];
    myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes * myIMU.magCalibration[1] -
               myIMU.magbias[1] - correction[7];
    myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes * myIMU.magCalibration[2] -
               myIMU.magbias[2] - correction[8];
               
    Serial.print('r');
    Serial.print(",");
    Serial.print(myIMU.ax);
    Serial.print(",");
    Serial.print(myIMU.ay);
    Serial.print(",");
    Serial.print(myIMU.az);
    Serial.print(",");
    Serial.print(myIMU.gx);
    Serial.print(",");
    Serial.print(myIMU.gy);
    Serial.print(",");
    Serial.print(myIMU.gz);
    Serial.print(",");
    Serial.print(myIMU.mx);
    Serial.print(",");
    Serial.print(myIMU.my);
    Serial.print(",");
    Serial.print(myIMU.mz);
    Serial.println();
    delay(10);
  }
}

