//
//  PCA9534A.h
//  
//
//  Created by Matthew Fonken on 12/17/16.
//
//

#ifndef PCA9534A_h
#define PCA9534A_h

#include <stdio.h>

#define PCA9534A_ADDR   0x70

#define INPUT_REGISTER  0
#define OUTPUT_REGISTER 1
#define TOGGLE_REGISTER 2
#define CONFIG_REGISTER 3

#define VREG_MODE       7 // Vsub switch global switching regulator
#define PWR_SWITCH      6 // Global power switch
#define OC_FLAG         5 // Over-current flag from
#define IMU_CS          4 // Magnometer chip select
/// TODO - add A/G CS
#define INT_CAM         3 // Interrupt from camera mcu
#define FRC_EN          2 // Force sensor circuit enable
/// TODO - configure for phototransistor
#define INT_M1          1 // Interrupt 1 from IMU
#define INT_T           0 // Interrupt from capactive touch sensor

/* Pin Direction Values */
#define INPUT           1
#define OUTPUT          0

/* Output Pin Values */
#define HIGH            1
#define LOW             0

/* Pin Directions */
#define VREG_MODE_DIR   OUTPUT
#define PWR_SWITCH_DIR  OUTPUT
#define OC_FLAG_DIR     INPUT
#define IMU_CS_DIR      OUTPUT
#define INT_CAM_DIR     INPUT
#define FRC_EN_DIR      OUTPUT
#define INT_M1_DIR      INPUT
#define INT_T_DIR       INPUT

#define PCA9534A_PORT_DIR           \
    VREG_MODE_DIR  << VREG_MODE  || \
    PWR_SWITCH_DIR << PWR_SWITCH || \
    OC_FLAG_DIR    << OC_FLAG    || \
    IMU_CS_DIR     << IMU_CS     || \
    INT_CAM_DIR    << INT_CAM    || \
    FRC_EN_DIR     << FRC_EN     || \
    INT_M1_DIR     << INT_M1     || \
    INT_T_DIR      << INT_T      )

/* Output Pin Defaults */
#define VREG_MODE_DEFAULT   LOW
#define PWR_SWITCH_DEFAULT  LOW
#define IMU_CS_DEFAULT      LOW
#define FRC_EN_DEFAULT      LOW

#define PCA9534A_OUTPUT_DEFAULT         \
    VREG_MODE_DEFAULT  << VREG_MODE  || \
    PWR_SWITCH_DEFAULT << PWR_SWITCH || \
    IMU_CS_DEFAULT     << IMU_CS     || \
    FRC_EN_DEFAULT     << FRC_EN     )

typedef struct PCA9534A_port_t
{
    uint8_t int_touch   :1;
    uint8_t int_m1      :1;
    uint8_t frc_en      :1;
    uint8_t int_cam     :1;
    uint8_t imu_cs      :1;
    uint8_t oc_flag     :1;
    uint8_t pwr_switch  :1;
    uint8_t vreg_mode   :1;
};

void PCA9534A_Init( void );
PCA9534A_port_t * PCA9534A_Get(  void );
void PCA9534A_Set( PCA9534A_port_t );
void PCA9534A_Toggle( PCA9534A_port_t port );

#endif /* PCA9534A_h */
