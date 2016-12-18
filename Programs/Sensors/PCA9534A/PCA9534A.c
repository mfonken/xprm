//
//  PCA9534A.c
//  
//
//  Created by Matthew Fonken on 12/17/16.
//
//

#include "PCA9534A.h"

void PCA9534A_Init(  void )
{
    uint8_t default_outputs = PCA9534A_OUTPUT_DEFAULT;
    PCA9534A_port_t default_port = *( PCA9534A_port_t *)&default_outputs;
    uint8_t i2c_write_data[] = { ( uint8_t )*port };
    I2C_Write( PCA9534A_ADDR, CONFIG_REGISTER, i2c_write_data, 1 );
}
PCA9534A_port_t * PCA9534A_Get(  void )
{
    
}

void PCA9534A_Set( PCA9534A_port_t port )
{
    uint8_t i2c_write_data[] = { ( uint8_t )*port };
    I2C_Write( PCA9534A_ADDR, OUTPUT_REGISTER, i2c_write_data, 1 );
}

void PCA9534A_Toggle( PCA9534A_port_t port )
{
    uint8_t i2c_write_data[] = { ( uint8_t )*port };
    I2C_Write( PCA9534A_ADDR, TOGGLE_REGISTER, i2c_write_data, 1 );
}
