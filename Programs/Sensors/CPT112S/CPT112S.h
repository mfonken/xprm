/***********************************************************************************************//**
* \file   CPT112S.h
* \brief  CPT112S Touch Controller Header
***************************************************************************************************
*      Author: Matthew Fonken
**************************************************************************************************/

#ifndef CPT112S_h
#define CPT112S_h

#include <stdio.h>
#include <stdint.h>

#define TOUCH_CTR_ADDR      0xe0

typedef struct
{
    uint32_t packet_counter :4;
    uint32_t type           :4;
    uint32_t description    :16;
} touch_ctr_event_t;

enum event_types
{
    TOUCH_EVENT     = 0,
    RELEASE_EVENT   = 1,
    SLIDER_ACTIVITY = 2,
};

uint8_t * touchControllerGet( void )
{
    uint8_t i2c_read_data[3];
    I2C_Read( TOUCH_CTR_ADDR, i2c_read_data, 3 );
    return i2c_read_data;
}

#endif /* CPT112S_h */
