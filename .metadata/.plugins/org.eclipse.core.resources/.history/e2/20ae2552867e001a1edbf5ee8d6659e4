
#ifndef __MY_LSM6DSL_H__

#define __MY_LSM6DSL_H__

#include "stm32f4xx_hal.h"


#include "lsm6dsl_reg.h"
#include <string.h>
#include <stdio.h>

typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;

typedef union{
  int16_t i16bit;
  uint8_t u8bit[2];
} axis1bit16_t;

axis3bit16_t data_raw_acceleration;
axis3bit16_t data_raw_angular_rate;
float acceleration_mg[3];
float angular_rate_mdps[3];
uint8_t whoamI, rst;

void lsm_init (void);
void lsm_callback (void);
int32_t lsm_write(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);
int32_t lsm_read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);


#endif // __MY_LSM6DSL_H__
