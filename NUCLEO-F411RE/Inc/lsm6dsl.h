/*
 * LSM6DSL.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

#ifndef MY_LSM6DSL_H
#define MY_LSM6DSL_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */

 void LSM6DSL_ProcessHanlder(void);

 void LSM6DSL_init (void);
 int32_t LSM6DSL_write(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);
 int32_t LSM6DSL_read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);

 void LSM6DSL_perSampleInit(void);
 void LSM6DSL_perSampleProcess (void);
 void LSM6DSL_perSampleDisable (void);

 void LSM6DSL_FIFO_Init(void);
 void LSM6DSL_FIFO_Process(void);
 void LSM6DSL_FIFO_Disable(void);
 void LSM6DSL_FIFO_Read_All_Data(void);
 void LSM6DSL_FIFO_Acc_And_Gyro_Read_Single_SAMPLE(uint16_t SampleIndex);
 void LSM6DSL_FIFO_Calc_Acc_Gyro_Avg_And_Print(uint16_t divider);
 void LSM6DSL_FIFO_Set_FIFO_Mode(void);
 void LSM6DSL_FIFO_Set_Bypass_Mode(void);
 void LSM6DSL_FIFO_Interrupt_Enable(void);
 void LSM6DSL_FIFO_Interrupt_Disable(void);
 void LSM6DSL_Mode_Disable(void);

 bool LSM6DSL_getInterruptFlag(void);
 void LSM6DSL_setInterruptFlagOn(void);
 void LSM6DSL_setInterruptFlagOff(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif // MY_LSM6DSL_H
