/***********************************************************************************************//**
* \file   mpu9250_regs.h
* \brief  MPU9250 registers initialization
***************************************************************************************************
*      Author: Matthew Fonken
**************************************************************************************************/


#include <stdio.h>
#include "mpu9250_regs.h"

/***************************************************************************************************
 Local Variables
 **************************************************************************************************/

/***************************************************************************************************
 Local Functions
 **************************************************************************************************/

/**************************************************************************//**
 * \brief Get register value from MPU9250
 * \param[out] Return value from register
 * \param[in] reg Register to access
 *****************************************************************************/
uint8_t mpu9250_GetRegister( uint8_t reg )
{
    uint8_t i2c_read_data[1];
    I2C_Read( IMU_ADDR, reg, i2c_read_data, 1 );
    return i2c_read_data[0];
}

/**************************************************************************//**
 * \brief Set register value on MPU9250
 * \param[in] reg Register to access
 * \param[in] val Value to set
 *****************************************************************************/
void mpu9250_SetRegister( uint8_t reg, uint8_t val )
{
    uint8_t i2c_write_data[2];
    i2c_write_data[0] = reg;
    i2c_write_data[0] = val;
    I2C_Write( IMU_ADDR, i2c_write_data, 2 );
}

/**************************************************************************//**
 * \brief Get register value from MPU9250 Magnometer
 * \param[out] Return value from register
 * \param[in] reg Register to access
 *****************************************************************************/
uint8_t mpu9250_GetMagRegister( uint8_t reg )
{
    uint8_t i2c_read_data[1];
    I2C_Read( MAG_ADDR, reg, i2c_read_data, 1 );
    return i2c_read_data[0];
}

/**************************************************************************//**
 * \brief Set register value on MPU9250 Magnometer
 * \param[in] reg Register to access
 * \param[in] val Value to set
 *****************************************************************************/
void mpu9250_SetMagRegister( uint8_t reg, uint8_t val )
{
    uint8_t i2c_write_data[2];
    i2c_write_data[0] = reg;
    i2c_write_data[0] = val;
    I2C_Write( MAG_ADDR, i2c_write_data, 2 );
}

/**************************************************************************//**
 * \brief Initialize MPU9250 with default values
 *****************************************************************************/
void mpu9250_defaultInit( mpu9250_global_t * this )
{
    this->config.general.fifoMode   = FIFO_MODE_DEFAULT;
    this->config.general.fsyncMode  = EXT_FSYNC_MODE_DEFAULT;
    this->config.general.dlpfConfig = GYRO_BW_DEFAULT;
    
    if( GYRO_BW_DEFAULT <= 0x07 )
    {
        this->config.gyro.fchoice  = GYRO_BW_ENABLE;
    }
    else
    {
        this->config.gyro.fchoice  = GYRO_BW_DEFAULT & GYRO_BW_MASK;
    }
    this->config.gyro.fscale        = GYRO_FS_DEFAULT;
    this->config.gyro.enable        = GYRO_EN_DEFAULT;
    
    this->config.accel.dlpfConfig   = ACCEL_BW_DEFAULT;
    if( ACCEL_BW_DEFAULT <= 0x07 )
    {
        this->config.accel.fchoice  = ENABLE;
    }
    else
    {
        this->config.accel.fchoice  = DISABLE;
    }
    this->config.accel.fscale       = ACCEL_FS_DEFAULT;
    this->config.accel.enable       = ACCEL_EN_DEFAULT;
    
    uint8_t  int_pin_config_default = INT_PIN_DEFAULT;
    uint8_t  accel_lp_odr_default   = ACCEL_LP_ODR_DEFAULT;
    uint8_t  accel_int_default      = ACCEL_INTEL_DEFAULT;
    uint8_t  fifo_en_default        = FIFO_ENABLE_DEFAULT;
    uint8_t  int_en_default         = INT_ENABLE_DEFAULT;
    uint8_t  int_status_default     = INT_STATUS_DEFAULT;
    uint8_t  usr_ctrl_default       = USER_CONFIG_DEFAULT;
    uint16_t pwr_mgmt_default       = PWR_MGMT_DEFAULT;
    
    this->config.interrupt          = *(mpu9250_int_pin_config_t  *)&int_pin_config_default;
    this->control.accel_lp          = *(mpu9250_accel_lp_odr_t    *)&accel_lp_odr_default;
    this->control.accel_int         = *(mpu9250_accel_interrupt_t *)&accel_int_default;
    this->control.fifo_en           = *(mpu9250_fifo_enable_t     *)&fifo_en_default;
    this->control.int_en            = *(mpu9250_int_enable_t      *)&int_en_default;
    this->control.int_status        = *(mpu9250_int_status_t      *)&int_status_default;
    this->control.usr_ctrl          = *(mpu9250_usr_control_t     *)&usr_ctrl_default;
    this->control.pwr_mgmt          = *(mpu9250_pwr_mgmt_t        *)&pwr_mgmt_default;
    
    mpu9250_updateRegisters( this );
}

/**************************************************************************//**
 * \brief Update MPU9250 registers with current global configuration
 *****************************************************************************/
void mpu9250_updateRegisters( mpu9250_global_t * this )
{
    mpu9250_SetRegister( CONFIG,          this->config.general           );
    mpu9250_SetRegister( GYRO_CONFIG,     this->config.gyro              );
    mpu9250_SetRegister( ACCEL_CONFIG_2,  this->config.accel << 0xFF     );
    mpu9250_SetRegister( ACCEL_CONFIG,    this->config.accel &  0xFF     );
    mpu9250_SetRegister( INT_PIN_CFG,     this->config.interrupt         );
    
    mpu9250_SetRegister( LP_ACCEL_ODR,    this->control.accel_lp         );
    mpu9250_SetRegister( MOT_DETECT_CTRL, this->control.accel_int        );
    mpu9250_SetRegister( FIFO_EN,         this->control.fifo_en          );
    mpu9250_SetRegister( INT_ENABLE,      this->control.int_en           );
    mpu9250_SetRegister( INT_STATUS,      this->control.int_status       );
    mpu9250_SetRegister( USER_CTRL,       this->control.usr_ctrl         );
    
    mpu9250_SetRegister( PWR_MGMT_2,      this->control.pwr_mgmt << 0xFF );
    mpu9250_SetRegister( PWR_MGMT_1,      this->control.pwr_mgmt &  0xFF );
}
