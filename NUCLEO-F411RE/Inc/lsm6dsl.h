
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

#define SAMPLES_TO_READ 								10
#define FIFO_SAMPLE_TO_BYTE_RATIO						2
#define ACC_SINGLE_FIFO_SAMPLE 							3
#define GYRO_SINGLE_FIFO_SAMPLE 						3
#define ACC_AND_GYRO_SINGLE_FIFO_SAMPLE 				(ACC_SINGLE_FIFO_SAMPLE + GYRO_SINGLE_FIFO_SAMPLE)
#define ACC_AND_GYRO_FIFO_WATERMARK   					(SAMPLES_TO_READ * ACC_AND_GYRO_SINGLE_FIFO_SAMPLE)
#define ACC_GYRO_BUF_BYTES_SIZE 						(ACC_AND_GYRO_FIFO_WATERMARK * FIFO_SAMPLE_TO_BYTE_RATIO)


axis3bit16_t data_raw_acc_gy_Buf[ACC_GYRO_BUF_BYTES_SIZE];

float acceleration_g_Sum[3];
float angular_rate_dps_Sum[3];


void LSM6DSL_Init (void);
int32_t LSM6DSL_Write(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);
int32_t LSM6DSL_Read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);

void LSM6DSL_Per_Sample_Init(void);
void LSM6DSL_Per_Sample_Process (void);
void LSM6DSL_Per_Sample_Disable (void);
void LSM6DSL_FIFO_Read_All_Data(void);
void LSM6DSL_FIFO_Acc_And_Gyro_Read_Single_SAMPLE(uint16_t SampleIndex);

void LSM6DSL_FIFO_Init(void);
void LSM6DSL_FIFO_Process(void);
void LSM6DSL_FIFO_Disable(void);
void LSM6DSL_FIFO_Set_FIFO_Mode(void);
void LSM6DSL_FIFO_Set_Bypass_Mode(void);
void LSM6DSL_FIFO_Calc_Acc_Gyro_Avg_And_Print(uint16_t divider);
void LSM6DSL_FIFO_Interrupt_Enable(void);
void LSM6DSL_FIFO_Interrupt_Disable(void);



#endif // __MY_LSM6DSL_H__
