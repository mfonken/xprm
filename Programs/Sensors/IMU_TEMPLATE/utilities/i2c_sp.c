/***********************************************************************************************//**
* \file   i2c_sp.c
* \brief  I2C Special Functions
***************************************************************************************************
*      Author: Matthew Fonken
**************************************************************************************************/

/* Own header */
#include "i2c_sp.h"

int file = 0xff;

bool I2C_Init( void )
{
	char *filename = (char*)"/dev/i2c-1";
	if ((file = open(filename, O_RDWR)) < 0) return 0;
	return 1;
}

bool I2C_Deinit( void )
{
    return close(file);
}

bool I2C_Read( uint8_t addr, uint8_t *i2c_read_data, uint8_t i2c_read_data_length)
{
    if( ioctl(file, I2C_SLAVE, addr) < 0 ) return 0;
    return read(file,i2c_read_data,i2c_read_data_length);
}

uint8_t I2C_Read_Reg( uint8_t addr, uint8_t reg)
{
    if( ioctl(file, I2C_SLAVE, addr) < 0 ) return -1;
    return i2c_smbus_read_byte_data(file, reg);
}

bool I2C_Read_Regs( uint8_t addr, uint8_t reg, uint8_t *i2c_read_data, uint8_t i2c_read_data_length)
{
	
	//uint8_t len = i2c_read_data_length - 1;
	//uint8_t temp[len];
	//bool ret = false;
    if( ioctl(file, I2C_SLAVE, addr) < 0 ) return 0;
	return i2c_smbus_read_i2c_block_data(file, reg, 6, i2c_read_data);
	//i2c_read_data[0] = i2c_smbus_read_byte_data(file, reg);
    //ret = read(file,temp,len);
	//for(int i = 0; i < len;i++) i2c_read_data[i+1] = temp[i];
	//return ret;
}

bool I2C_Write( uint8_t addr, uint8_t *i2c_write_data, uint8_t i2c_write_data_length)
{
    if( ioctl(file, I2C_SLAVE, addr) < 0 ) return 0;
    return write(file, i2c_write_data, i2c_write_data_length);
}
