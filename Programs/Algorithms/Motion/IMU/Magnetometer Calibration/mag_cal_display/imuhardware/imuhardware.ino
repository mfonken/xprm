#include <Wire.h>

#include <FaBo9Axis_MPU9250.h>
#define SerialDebug false  // Set to true to get Serial output for debugging

#define SRV       9

#define BFR_SIZE  15
float sampleBuffer[BFR_SIZE][9];
int bufferIndex = 0;
float bufferSum[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

// Pin definitions
int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int myLed  = 13;  // Set up pin 13 led for toggling

FaBo9Axis myIMU;

float mb[3] = {0, 0, 0};

float correction[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(10);
  Wire.begin();

  if (myIMU.begin()) // WHO_AM_I should always be 0x68
  {
    Serial.println("MPU9250 is online...");
  }
  else
  {
    Serial.print("Could not connect to MPU9250");
    while (1);
  }
}

void loop()
{
  float mx, my, mz;
  myIMU.readMagnetXYZ(&mx, &my, &mz);
  Serial.print('m');
  Serial.print(",");
  Serial.print(mx);
  Serial.print(",");
  Serial.print(my);
  Serial.print(",");
  Serial.print(mz);
  Serial.println();
  delay(100);
}

