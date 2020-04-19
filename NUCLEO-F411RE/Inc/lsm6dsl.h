
#ifndef __MY_LSM6DSL_H__

#define __MY_LSM6DSL_H__

#include "stm32f4xx_hal.h"


#include "lsm6dsl_reg.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;

axis3bit16_t data_raw_acceleration;
axis3bit16_t data_raw_angular_rate;
float acceleration_mg[3];
float angular_rate_mdps[3];
uint8_t whoamI, rst;
bool int1_occurred;


#define SAMPLES_TO_READ 10
#define FIFO_WATERMARK_ACC_GY   SAMPLES_TO_READ * 6 + 1


axis3bit16_t data_raw_acceleration_Buf[SAMPLES_TO_READ];
axis3bit16_t data_raw_angular_rate_Buf[SAMPLES_TO_READ];
float acceleration_g_Sum[3];
float angular_rate_dps_Sum[3];
//float acceleration_mg_Buf[SAMPLES_TO_READ][3];
//float angular_rate_mdps_Buf[SAMPLES_TO_READ][3];


void LSM6DSL_INIT (void);
int32_t LSM6DSL_WRITE(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);
int32_t LSM6DSL_READ(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);
void LSM6DSL_PER_SAMPLE_Init(void);
void LSM6DSL_PER_SAMPLE_PROCESS (void);
void LSM6DSL_PER_SAMPLE_DISABLE (void);
void LSM6DSL_FIFOMode_Init(void);
void LSM6DSL_FIFOMode_Process(void);
void LSM6DSL_FIFOMode_DISABLE(void);


void LSM6DSL_Read_All_FIFO_Data(void);
void LSM6DSL_Read_Single_FIFO_SAMPLE(uint16_t SampleIndex);
void LSM6DSL_FIFO_Set_FIFO_Mode(void);
void LSM6DSL_FIFO_Set_Bypass_Mode(void);


#endif // __MY_LSM6DSL_H__
