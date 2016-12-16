/***********************************************************************************************//**
* \file   LSM9DS1_regs.c
* \brief  LSM9DS1 registers initialization
***************************************************************************************************
*      Author: Matthew Fonken
**************************************************************************************************/

#include <stdio.h>
#include "LSM9DS1_regs.h"

/***************************************************************************************************
Local Variables
**************************************************************************************************/

/***************************************************************************************************
Local Functions
**************************************************************************************************/

/**************************************************************************//**
* \brief Get register value from LSM9DS1
* \param[out] Return value from register
* \param[in] reg Register to access
*****************************************************************************/
uint8_t LSM9DS1_GetRegister( uint8_t reg )
{
    uint8_t i2c_read_data[1];
    I2C_Read( LSM9DS1_IMU_ADDR, reg, i2c_read_data, 1 );
    return i2c_read_data[0];
}

/**************************************************************************//**
* \brief Set register value on LSM9DS1
* \param[in] reg Register to access
* \param[in] val Value to set
*****************************************************************************/
void LSM9DS1_SetRegister( uint8_t reg, uint8_t val )
{
    uint8_t i2c_write_data[2];
    i2c_write_data[0] = reg;
    i2c_write_data[0] = val;
    I2C_Write( LSM9DS1_IMU_ADDR, i2c_write_data, 2 );
}

/**************************************************************************//**
* \brief Get register value from LSM9DS1 Magnometer
* \param[out] Return value from register
* \param[in] reg Register to access
*****************************************************************************/
uint8_t LSM9DS1_GetMagRegister( uint8_t reg )
{
    uint8_t i2c_read_data[1];
    I2C_Read( LSM9DS1_MAG_ADDR, reg, i2c_read_data, 1 );
    return i2c_read_data[0];
}

/**************************************************************************//**
* \brief Set register value on LSM9DS1 Magnometer
* \param[in] reg Register to access
* \param[in] val Value to set
*****************************************************************************/
void LSM9DS1_SetMagRegister( uint8_t reg, uint8_t val )
{
    uint8_t i2c_write_data[2];
    i2c_write_data[0] = reg;
    i2c_write_data[0] = val;
    I2C_Write( LSM9DS1_MAG_ADDR, i2c_write_data, 2 );
}

/**************************************************************************//**
* \brief Initialize LSM9DS1 with default values
*****************************************************************************/
void LSM9DS1_defaultInit( LSM9DS1_cfg_t * this )
{
    
    uint8_t act_ths_default             = ACT_THS_DEFAULT;
    uint8_t act_dur_default             = ACT_DUR_DEFAULT;
    uint8_t int1_ctrl_default           = INT1_CTRL_DEFAULT;
    uint8_t int2_ctrl_default           = INT2_CTRL_DEFAULT;
    uint8_t ctrl_reg8_default           = CTRL_REG8_DEFAULT;
    uint8_t ctrl_reg9_default           = CTRL_REG9_DEFAULT;
    uint8_t ctrl_reg10_default          = CTRL_REG10_DEFAULT;
    this->general.activity_threshold    = *( LSM9DS1_act_ths_t    *)&act_ths_default;
    this->general.inactivity_duration   = *( LSM9DS1_act_dur_t    *)&act_dur_default;
    this->general.int1_control          = *( LSM9DS1_int1_ctrl_t  *)&int1_ctrl_default;
    this->general.int2_control          = *( LSM9DS1_int2_ctrl_t  *)&int2_ctrl_default;
    this->general.system_control        = *( LSM9DS1_ctrl_reg8_t  *)&ctrl_reg8_default;
    this->general.system_enables        = *( LSM9DS1_ctrl_reg9_t  *)&ctrl_reg9_default;
    this->general.self_test             = *( LSM9DS1_ctrl_reg10_t *)&ctrl_reg10_default;
    
    uint8_t fifo_ctrl_default           = FIFO_CTRL_DEFAULT;
    uint8_t fifo_src_default            = FIFO_SRC_DEFAULT;
    this->fifo.control                  = *( LSM9DS1_fifo_ctrl_t  *)&fifo_ctrl_default;
    this->fifo.status_control           = *( LSM9DS1_fifo_src_t   *)&fifo_src_default;
    
    uint8_t int_gen_ths_x_xl_default    = INT_GEN_THS_X_XL_DEFAULT;
    uint8_t int_gen_ths_y_xl_default    = INT_GEN_THS_Y_XL_DEFAULT;
    uint8_t int_gen_ths_z_xl_default    = INT_GEN_THS_Z_XL_DEFAULT;
    uint8_t int_gen_cfg_xl_default      = INT_GEN_CFG_XL_DEFAULT;
    uint8_t int_gen_dur_xl_default      = INT_GEN_DUR_XL_DEFAULT;
    uint8_t ctrl_reg6_xl_default        = CTRL_REG6_XL_DEFAULT;
    uint8_t ctrl_reg7_xl_default        = CTRL_REG7_XL_DEFAULT;
    uint8_t ctrl_reg5_xl_default        = CTRL_REG5_XL_DEFAULT;
    this->accel.interrupt_threshold_x   = *( LSM9DS1_int_gen_ths_x_xl_t *)&int_gen_ths_x_xl_default;
    this->accel.interrupt_threshold_y   = *( LSM9DS1_int_gen_ths_y_xl_t *)&int_gen_ths_y_xl_default;
    this->accel.interrupt_threshold_z   = *( LSM9DS1_int_gen_ths_z_xl_t *)&int_gen_ths_z_xl_default;
    this->accel.interrupt_configuration = *( LSM9DS1_int_gen_cfg_xl_t   *)&int_gen_cfg_xl_default;
    this->accel.interrupt_duration      = *( LSM9DS1_int_gen_dur_xl_t   *)&int_gen_dur_xl_default;
    this->accel.bw_odr_control          = *( LSM9DS1_ctrl_reg6_xl_t     *)&ctrl_reg6_xl_default;
    this->accel.filter_control          = *( LSM9DS1_ctrl_reg7_xl_t     *)&ctrl_reg7_xl_default;
    this->accel.enable                  = *( LSM9DS1_ctrl_reg5_t        *)&ctrl_reg5_xl_default;
    
    uint8_t ctrl_reg1_g_default         = CTRL_REG1_G_DEFAULT;
    uint8_t ctrl_reg2_g_default         = CTRL_REG2_G_DEFAULT;
    uint8_t ctrl_reg3_g_default         = CTRL_REG3_G_DEFAULT;
    uint16_t int_gen_ths_x_g_default    = ( INT_GEN_THS_XH_G_DEFAULT << 8 ) && INT_GEN_THS_XL_G_DEFAULT;
    uint16_t int_gen_ths_y_g_default    = ( INT_GEN_THS_YH_G_DEFAULT << 8 ) && INT_GEN_THS_YL_G_DEFAULT;
    uint16_t int_gen_ths_z_g_default    = ( INT_GEN_THS_ZH_G_DEFAULT << 8 ) && INT_GEN_THS_ZL_G_DEFAULT;
    uint8_t orient_cfg_g_default        = ORIENT_CFG_G_DEFAULT;
    uint8_t int_gen_cfg_g_default       = INT_GEN_CFG_G_DEFAULT;
    uint8_t int_gen_dur_g_default       = INT_GEN_DUR_G_DEFAULT;
    uint8_t ctrl_reg4_default           = CTRL_REG4_DEFAULT;
    this->gyro.ctrl1                    = *( LSM9DS1_ctrl_reg1_g_t     *)&ctrl_reg1_g_default;
    this->gyro.ctrl2                    = *( LSM9DS1_ctrl_reg2_g_t     *)&ctrl_reg2_g_default;
    this->gyro.ctrl3                    = *( LSM9DS1_ctrl_reg3_g_t     *)&ctrl_reg3_g_default;
    this->gyro.interrupt_threshold_x    = *( LSM9DS1_int_gen_ths_x_g_t *)&int_gen_ths_x_g_default;
    this->gyro.interrupt_threshold_y    = *( LSM9DS1_int_gen_ths_y_g_t *)&int_gen_ths_y_g_default;
    this->gyro.interrupt_threshold_z    = *( LSM9DS1_int_gen_ths_z_g_t *)&int_gen_ths_z_g_default;
    this->gyro.orientation              = *( LSM9DS1_orient_cfg_g_t    *)&orient_cfg_g_default;
    this->gyro.interrupt_configuration  = *( LSM9DS1_int_gen_cfg_g_t   *)&int_gen_cfg_g_default;
    this->gyro.interrupt_duration       = *( LSM9DS1_int_gen_dur_g_t   *)&int_gen_dur_g_default;
    this->gyro.enable                   = *( LSM9DS1_ctrl_reg4_t       *)&ctrl_reg4_default;
    
    uint16_t offset_x_reg_m_default     = ( OFFSET_X_REG_H_M_DEFAULT << 8 ) && OFFSET_X_REG_L_M_DEFAULT;
    uint16_t offset_y_reg_m_default     = ( OFFSET_Y_REG_H_M_DEFAULT << 8 ) && OFFSET_Y_REG_L_M_DEFAULT;
    uint16_t offset_z_reg_m_default     = ( OFFSET_Z_REG_H_M_DEFAULT << 8 ) && OFFSET_Z_REG_L_M_DEFAULT;
    uint8_t ctrl_reg1_m_default         = CTRL_REG1_M_DEFAULT;
    uint8_t ctrl_reg2_m_default         = CTRL_REG2_M_DEFAULT;
    uint8_t ctrl_reg3_m_default         = CTRL_REG3_M_DEFAULT;
    uint8_t ctrl_reg4_m_default         = CTRL_REG4_M_DEFAULT;
    uint8_t ctrl_reg5_m_default         = CTRL_REG5_M_DEFAULT;
    uint8_t int_cfg_m_default           = INT_CFG_M_DEFAULT;
    uint16_t int_ths_m_default          = ( INT_THS_H_M_DEFAULT << 8 ) && INT_THS_L_M_DEFAULT;
    this->mag.x_offset                  = *( LSM9DS1_offset_x_reg_m_t *)&offset_x_reg_m_default;
    this->mag.y_offset                  = *( LSM9DS1_offset_y_reg_m_t *)&offset_y_reg_m_default;
    this->mag.z_offset                  = *( LSM9DS1_offset_z_reg_m_t *)&offset_z_reg_m_default;
    this->mag.odr_control               = *( LSM9DS1_ctrl_reg1_m_t    *)&ctrl_reg1_m_default;
    this->mag.fs_control                = *( LSM9DS1_ctrl_reg2_m_t    *)&ctrl_reg2_m_default;
    this->mag.configuration_control     = *( LSM9DS1_ctrl_reg3_m_t    *)&ctrl_reg3_m_default;
    this->mag.operative_mode            = *( LSM9DS1_ctrl_reg4_m_t    *)&ctrl_reg4_m_default;
    this->mag.block_data_update_control = *( LSM9DS1_ctrl_reg5_m_t    *)&ctrl_reg5_m_default;
    this->mag.interrupt_configuration   = *( LSM9DS1_int_cfg_m_t      *)&int_cfg_m_default;
    this->mag.interrupt_threshold       = *( LSM9DS1_int_ths_t        *)&int_ths_m_default;
    
    LSM9DS1_updateRegisters( this );
}

/**************************************************************************//**
* \brief Update LSM9DS1 registers with current global configuration
*****************************************************************************/
void LSM9DS1_updateRegisters( LSM9DS1_cfg_t * this )
{
    LSM9DS1_SetRegister( ACT_THS,           this->general.activity_threshold);
    LSM9DS1_SetRegister( ACT_DUR,           this->general.inactivity_duration);
    LSM9DS1_SetRegister( INT1_CTRL,         this->general.int1_control);
    LSM9DS1_SetRegister( INT2_CTRL,         this->general.int2_control);
    LSM9DS1_SetRegister( CTRL_REG8,         this->general.system_control);
    LSM9DS1_SetRegister( CTRL_REG9,         this->general.system_enables);
    LSM9DS1_SetRegister( CTRL_REG10,        this->general.self_test);
    
    LSM9DS1_SetRegister( FIFO_CTRL,         this->fifo.control);
    LSM9DS1_SetRegister( FIFO_SRC,          this->fifo.status_control);
    
    LSM9DS1_SetRegister( INT_GEN_THS_X_XL,  this->accel.interrupt_threshold_x);
    LSM9DS1_SetRegister( INT_GEN_THS_Y_XL,  this->accel.interrupt_threshold_y);
    LSM9DS1_SetRegister( INT_GEN_THS_Z_XL,  this->accel.interrupt_threshold_z);
    LSM9DS1_SetRegister( INT_GEN_CFG_XL,    this->accel.interrupt_configuration);
    LSM9DS1_SetRegister( INT_GEN_DUR_XL,    this->accel.interrupt_duration);
    LSM9DS1_SetRegister( CTRL_REG6_XL,      this->accel.bw_odr_control);
    LSM9DS1_SetRegister( CTRL_REG7_XL,      this->accel.filter_control);
    LSM9DS1_SetRegister( CTRL_REG5_XL,      this->accel.enable);
    
    LSM9DS1_SetRegister( CTRL_REG1_G,       this->gyro.ctrl1);
    LSM9DS1_SetRegister( CTRL_REG2_G,       this->gyro.ctrl2);
    LSM9DS1_SetRegister( CTRL_REG3_G,       this->gyro.ctrl3);
    LSM9DS1_SetRegister( INT_GEN_THS_XH_G,  this->gyro.interrupt_threshold_x << 8 );
    LSM9DS1_SetRegister( INT_GEN_THS_XL_G,  this->gyro.interrupt_threshold_x);
    LSM9DS1_SetRegister( INT_GEN_THS_YH_G,  this->gyro.interrupt_threshold_y << 8 );
    LSM9DS1_SetRegister( INT_GEN_THS_YL_G,  this->gyro.interrupt_threshold_y);
    LSM9DS1_SetRegister( INT_GEN_THS_ZH_G,  this->gyro.interrupt_threshold_z << 8 );
    LSM9DS1_SetRegister( INT_GEN_THS_ZL_G,  this->gyro.interrupt_threshold_z);
    LSM9DS1_SetRegister( ORIENT_CFG_G,      this->gyro.orientation);
    LSM9DS1_SetRegister( INT_GEN_CFG_G,     this->gyro.interrupt_configuration);
    LSM9DS1_SetRegister( INT_GEN_DUR_G,     this->gyro.interrupt_duration);
    LSM9DS1_SetRegister( CTRL_REG4,         this->gyro.enable);
    
    LSM9DS1_SetRegister( OFFSET_X_REG_H_M,  this->mag.x_offset << 8 );
    LSM9DS1_SetRegister( OFFSET_X_REG_L_M,  this->mag.x_offset);
    LSM9DS1_SetRegister( OFFSET_Y_REG_H_M,  this->mag.y_offset << 8 );
    LSM9DS1_SetRegister( OFFSET_Y_REG_L_M,  this->mag.y_offset);
    LSM9DS1_SetRegister( OFFSET_Z_REG_H_M,  this->mag.z_offset << 8 );
    LSM9DS1_SetRegister( OFFSET_Z_REG_L_M,  this->mag.z_offset);
    LSM9DS1_SetRegister( CTRL_REG1_M,       this->mag.odr_control);
    LSM9DS1_SetRegister( CTRL_REG2_M,       this->mag.fs_control);
    LSM9DS1_SetRegister( CTRL_REG3_M,       this->mag.configuration_control);
    LSM9DS1_SetRegister( CTRL_REG4_M,       this->mag.operative_mode);
    LSM9DS1_SetRegister( CTRL_REG5_M,       this->mag.block_data_update_control);
    LSM9DS1_SetRegister( INT_CFG_M,         this->mag.interrupt_configuration);
    LSM9DS1_SetRegister( INT_THS_H_M,       this->mag.interrupt_threshold << 8 );
    LSM9DS1_SetRegister( INT_THS_L_M,       this->mag.interrupt_threshold ACT_THS,);
}
