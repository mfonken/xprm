//
//  packing_test.c
//  
//
//  Created by Matthew Fonken on 11/26/16.
//
//

#include <stdio.h>
#include <stdint.h>
#include "mpu9250_regs.c"

typedef struct
{
    uint8_t a   :1;
    uint8_t b   :1;
    uint8_t c   :1;
    uint8_t d   :1;
    uint8_t e   :1;
    uint8_t f   :1;
    uint8_t g   :1;
    uint8_t h   :1;
} packed_t;

int main(int argc, char * args[])
{
    packed_t t;
    
    t.a = 1;
    t.b = 1;
    t.c = 1;
    t.d = 1;
    t.e = 1;
    t.f = 1;
    t.g = 1;
    t.h = 0;

    uint8_t *p = (uint8_t *)&t;
    uint8_t  u = *p;
    uint16_t w;
    uint64_t l;
    
    for(int i = 0; i < 8; i++)
    {
        //printf("%d> %d\n", i, *(pointer+i));
    }
    printf("Real pointer is %p and p is %p\n", &t, p);
    printf("Type is: 0x%02X of size: %lu byte\n", u, sizeof(packed_t));
    
    printf("\n---Testing mpu9520_regs types---\n");
    
//    mpu9250_gen_config_t gen_config;
//    mpu9250_gyro_config_t gyro_config;
//    mpu9250_accel_config_t accel_config;
//    mpu9250_int_pin_config_t int_pin_config;
    
    mpu9250_global_t global;
    
    *(uint8_t *)&global.config.general = 0;
    *(uint8_t *)&global.config.gyro = 0;
    *(uint16_t *)&global.config.accel = 0;
    *(uint8_t *)&global.config.interrupt = 0;
//    gen_config.fifoMode = 1;
//    gyro_config.enable  = 7;
//    accel_config.enable = 7;
//    int_pin_config.actl = 1;
    
    mpu9250_defaultInit( &global );
    
    u = *(uint8_t *)&global.config.general;
    printf("Type is: 0x%02X of size: %lu byte\n", u, sizeof(mpu9250_gen_config_t));
    

    u = *(uint8_t *)&global.config.gyro;
    printf("Type is: 0x%02X of size: %lu byte\n", u, sizeof(mpu9250_gyro_config_t));
    

    w = *(uint16_t *)&global.config.accel;
    printf("Type is: 0x%04X of size: %lu byte\n", w, sizeof(mpu9250_accel_config_t));
    
    u = *(uint8_t *)&global.config.interrupt;
    printf("Type is: 0x%02X of size: %lu byte\n", u, sizeof(mpu9250_int_pin_config_t));
    




    l = *(uint64_t *)&global.config;
    printf("Type is: 0x%12llX of size: %lu byte\n", l, sizeof(mpu9250_config_t));
    
    return 0;
}
