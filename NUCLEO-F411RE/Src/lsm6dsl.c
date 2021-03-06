/*
 * lsm6dsl.c
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

/******************************************************************************
* Includes
*******************************************************************************/

#include "lsm6dsl.h"
#include "lsm6dsl_reg.h"
#include "com.h"
#include "usart.h"
#include "i2c.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

#define SAMPLES_TO_READ 								10
#define FIFO_SAMPLE_TO_BYTE_RATIO						2
#define ACC_SINGLE_FIFO_SAMPLE 							3
#define GYRO_SINGLE_FIFO_SAMPLE 						3
#define ACC_AND_GYRO_SINGLE_FIFO_SAMPLE 				(ACC_SINGLE_FIFO_SAMPLE + GYRO_SINGLE_FIFO_SAMPLE)
#define ACC_AND_GYRO_FIFO_WATERMARK   					(SAMPLES_TO_READ * ACC_AND_GYRO_SINGLE_FIFO_SAMPLE)
#define ACC_GYRO_BUF_BYTES_SIZE 						(ACC_AND_GYRO_FIFO_WATERMARK * FIFO_SAMPLE_TO_BYTE_RATIO)

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

typedef union {
	int16_t i16bit[3];
	uint8_t u8bit[6];
} axis3bit16_t;

typedef enum {
	LSM6DSL_MODE_IDLE = 0,
	LSM6DSL_MODE_PER_SAMPLE = 1,
	LSM6DSL_MODE_FIFO = 2,
} lsm6dsl_mode_t;

typedef enum {
    LSM6DSL_CONNECTED = 0,
	LSM6DSL_DISCONNECTED = 1,
} lsm6dsl_is_connected_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

static axis3bit16_t Data_Raw_Acceleration;
static axis3bit16_t Data_Raw_Angular_Rate;
static float P_Acceleration_Mg[3];
static float P_Angular_Rate_Mdps[3];
static uint8_t Who_Am_I, Rst;

static axis3bit16_t P_Data_Raw_Acc_Gy_Buf[ACC_GYRO_BUF_BYTES_SIZE];

static float P_Acceleration_G_Sum[3];
static float P_Angular_Rate_Dps_Sum[3];

static stmdev_ctx_t Dev_Ctx;
static char Data[100];

static lsm6dsl_mode_t Lsm6dsl_Mode;
static lsm6dsl_is_connected_t Lsm6dsl_Is_Connected;

static bool Interrup_Flag;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

static int32_t LSM6DSL_write(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);
static int32_t LSM6DSL_read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);
static void LSM6DSL_perSampleProcess (void);
static void LSM6DSL_perSampleDisable (void);
static void LSM6DSL_fifoProcess(void);
static void LSM6DSL_fifoDisable(void);
static void LSM6DSL_fifoReadAllData(void);
static void LSM6DSL_fifoAccAndGyroReadSingleSample(uint16_t SampleIndex);
static void LSM6DSL_fifoCalcAccGyroAvgAndPrint(uint16_t divider);
static void LSM6DSL_fifoSetFIFOMode(void);
static void LSM6DSL_fifoSetBypassMode(void);
static void LSM6DSL_fifoInterruptEnable(void);
static void LSM6DSL_fifoInterruptDisable(void);


/******************************************************************************
* Function Definitions
*******************************************************************************/

void LSM6DSL_processHanlder(void)
{
	if (Lsm6dsl_Mode == LSM6DSL_MODE_PER_SAMPLE)
	{
		LSM6DSL_perSampleProcess();
	}
	else if (Lsm6dsl_Mode == LSM6DSL_MODE_FIFO)
	{
		LSM6DSL_fifoProcess();
	}
}

int32_t LSM6DSL_write(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
    HAL_I2C_Mem_Write(handle, LSM6DSL_I2C_ADD_H, Reg, I2C_MEMADD_SIZE_8BIT, Bufp, len, 1000);
    return 0;
}

int32_t LSM6DSL_read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
	HAL_I2C_Mem_Read(handle, LSM6DSL_I2C_ADD_H, Reg, I2C_MEMADD_SIZE_8BIT, Bufp, len, 1000);
	return 0;
}

void LSM6DSL_init (void)
{
	Lsm6dsl_Is_Connected = LSM6DSL_DISCONNECTED;

	/* Initialize mems driver interface */
	Dev_Ctx.write_reg = LSM6DSL_write;
	Dev_Ctx.read_reg = LSM6DSL_read;
	Dev_Ctx.handle = &hi2c1;

	/* Check device ID */
	Who_Am_I = 0;
	lsm6dsl_device_id_get(&Dev_Ctx, &Who_Am_I);
	if ( Who_Am_I != LSM6DSL_ID )
	{
		COM_uartPrint("lsm6dsl device not found\n");
		return;
		//while(1); /* device not found */
	}

	/* Restore default configuration */
	lsm6dsl_reset_set(&Dev_Ctx, PROPERTY_ENABLE);
	do {
	lsm6dsl_reset_get(&Dev_Ctx, &Rst);
	} while (Rst);

	/* Enable Block Data Update */
	lsm6dsl_block_data_update_set(&Dev_Ctx, PROPERTY_ENABLE);

	/* Set Output Data Rate */
	lsm6dsl_xl_data_rate_set(&Dev_Ctx, LSM6DSL_XL_ODR_12Hz5);
	lsm6dsl_gy_data_rate_set(&Dev_Ctx, LSM6DSL_GY_ODR_12Hz5);

	/* Set full scale */
	lsm6dsl_xl_full_scale_set(&Dev_Ctx, LSM6DSL_8g);
	lsm6dsl_gy_full_scale_set(&Dev_Ctx, LSM6DSL_500dps);

	/* Accelerometer - analog filter */
	lsm6dsl_xl_filter_analog_set(&Dev_Ctx, LSM6DSL_XL_ANA_BW_400Hz);

	/* Accelerometer - LPF1 + LPF2 path */
	lsm6dsl_xl_lp2_bandwidth_set(&Dev_Ctx, LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);

	/* Gyroscope - filtering chain */
	lsm6dsl_gy_band_pass_set(&Dev_Ctx, LSM6DSL_HP_260mHz_LP1_STRONG);

	Lsm6dsl_Mode = LSM6DSL_MODE_IDLE;

	Lsm6dsl_Is_Connected = LSM6DSL_CONNECTED;
}

void LSM6DSL_perSampleInit(void)
{

	if (Lsm6dsl_Is_Connected == LSM6DSL_DISCONNECTED)
	{
		COM_uartPrint("LSM6DSL is not connected!\n");
		return;
	}
	if (Lsm6dsl_Mode == LSM6DSL_MODE_FIFO)
	{
		COM_uartPrint("Need to disable FIFO mode first\n");
		return;
	}

	//LSM6DSL_ACC_Set_INT1_DRDY
	//LSM6DSL_GYRO_Set_INT1_DRDY

	lsm6dsl_pin_polarity_set(&Dev_Ctx, LSM6DSL_ACTIVE_LOW);

	lsm6dsl_data_ready_mode_set(&Dev_Ctx, LSM6DSL_DRDY_PULSED);

	lsm6dsl_int1_route_t reg;

	lsm6dsl_pin_int1_route_get(&Dev_Ctx, &reg);

	reg.int1_drdy_xl = PROPERTY_ENABLE;
	reg.int1_drdy_g = PROPERTY_ENABLE;

	lsm6dsl_pin_int1_route_set(&Dev_Ctx, reg);

	Lsm6dsl_Mode = LSM6DSL_MODE_PER_SAMPLE;
}

void LSM6DSL_perSampleProcess (void)
{
	/*
	 * Read output only if new value is available
	 */
	lsm6dsl_reg_t reg;
	lsm6dsl_status_reg_get(&Dev_Ctx, &reg.status_reg);


	if (reg.status_reg.xlda)
	{
	  memset(Data_Raw_Acceleration.u8bit, 0x00, 3*sizeof(int16_t));
	  lsm6dsl_acceleration_raw_get(&Dev_Ctx, Data_Raw_Acceleration.u8bit);
	  P_Acceleration_Mg[0] = lsm6dsl_from_fs8g_to_mg( Data_Raw_Acceleration.i16bit[0]);
	  P_Acceleration_Mg[1] = lsm6dsl_from_fs8g_to_mg( Data_Raw_Acceleration.i16bit[1]);
	  P_Acceleration_Mg[2] = lsm6dsl_from_fs8g_to_mg( Data_Raw_Acceleration.i16bit[2]);

	  sprintf(Data, "Acceleration [mg]:  %4.2f\t%4.2f\t%4.2f\n",
	  			  P_Acceleration_Mg[0], P_Acceleration_Mg[1], P_Acceleration_Mg[2]);

	  COM_uartPrint(Data);
	}
	if (reg.status_reg.gda)
	{
	  memset(Data_Raw_Angular_Rate.u8bit, 0x00, 3*sizeof(int16_t));
	  lsm6dsl_angular_rate_raw_get(&Dev_Ctx, Data_Raw_Angular_Rate.u8bit);
	  P_Angular_Rate_Mdps[0] = lsm6dsl_from_fs500dps_to_mdps(Data_Raw_Angular_Rate.i16bit[0]);
	  P_Angular_Rate_Mdps[1] = lsm6dsl_from_fs500dps_to_mdps(Data_Raw_Angular_Rate.i16bit[1]);
	  P_Angular_Rate_Mdps[2] = lsm6dsl_from_fs500dps_to_mdps(Data_Raw_Angular_Rate.i16bit[2]);

	  sprintf(Data, "Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\n\n",
			  P_Angular_Rate_Mdps[0], P_Angular_Rate_Mdps[1], P_Angular_Rate_Mdps[2]);

	  COM_uartPrint(Data);
	}
}

void LSM6DSL_perSampleDisable (void)
{
	lsm6dsl_int1_route_t reg;

	lsm6dsl_pin_int1_route_get(&Dev_Ctx, &reg);

	reg.int1_drdy_xl = PROPERTY_DISABLE;
	reg.int1_drdy_g = PROPERTY_DISABLE;

	lsm6dsl_pin_int1_route_set(&Dev_Ctx, reg);
}

void LSM6DSL_fifoInit(void)
{
	if (Lsm6dsl_Is_Connected == LSM6DSL_DISCONNECTED)
	{
		COM_uartPrint("LSM6DSL is not connected!\n");
		return;
	}
	if (Lsm6dsl_Mode == LSM6DSL_MODE_PER_SAMPLE)
	{
		COM_uartPrint("Need to disable PER_SAMPLE mode first\n");
		return;
	}

	lsm6dsl_pin_polarity_set(&Dev_Ctx, LSM6DSL_ACTIVE_LOW);

	/* Set acc&gyro FIFO decimation */
	lsm6dsl_dec_fifo_xl_t valXl;
	lsm6dsl_dec_fifo_gyro_t valGy;

	lsm6dsl_fifo_xl_batch_get(&Dev_Ctx, &valXl);
	valXl = LSM6DSL_FIFO_XL_NO_DEC;
	lsm6dsl_fifo_xl_batch_set(&Dev_Ctx, valXl);

	lsm6dsl_fifo_gy_batch_get(&Dev_Ctx, &valGy);
	valGy = LSM6DSL_FIFO_GY_NO_DEC;
	lsm6dsl_fifo_gy_batch_set(&Dev_Ctx, valGy);

	/* Set FIFO ODR value */
	lsm6dsl_odr_fifo_t valOdr;
	lsm6dsl_fifo_data_rate_get(&Dev_Ctx, &valOdr);
	valOdr = LSM6DSL_FIFO_12Hz5;
	lsm6dsl_fifo_data_rate_set(&Dev_Ctx, valOdr);

	LSM6DSL_fifoInterruptEnable();

	/* Set FIFO watermark */
	uint16_t valWtm;
	lsm6dsl_fifo_watermark_get(&Dev_Ctx, &valWtm);
	valWtm = ACC_AND_GYRO_FIFO_WATERMARK;
	lsm6dsl_fifo_watermark_set(&Dev_Ctx, valWtm);

	/* Set FIFO depth to be limited to watermark threshold level  */
	/*uint8_t valStopOnWtm;
	lsm6dsl_fifo_stop_on_wtm_get(&Dev_Ctx, &valStopOnWtm);
	valStopOnWtm = PROPERTY_ENABLE;
	lsm6dsl_fifo_stop_on_wtm_set(&Dev_Ctx, valStopOnWtm);*/

	LSM6DSL_fifoSetFIFOMode();

	Lsm6dsl_Mode = LSM6DSL_MODE_FIFO;
}

void LSM6DSL_fifoProcess(void)
{
	LSM6DSL_fifoReadAllData();
}

void LSM6DSL_fifoDisable(void)
{
	LSM6DSL_fifoInterruptDisable();
	LSM6DSL_fifoSetBypassMode();
}

void LSM6DSL_fifoReadAllData(void)
{
	uint16_t unread_int16_fifo_samples = 0;

	for(int i = 0; i < 3; i++)
	{
		P_Acceleration_G_Sum[i] = 0;
		P_Angular_Rate_Dps_Sum[i] = 0;
	}

	/* Get num of unread FIFO samples before reading Data */
	lsm6dsl_fifo_data_level_get(&Dev_Ctx, &unread_int16_fifo_samples);

	uint16_t unread_acc_and_gyro_samples = unread_int16_fifo_samples / ACC_AND_GYRO_SINGLE_FIFO_SAMPLE;
	sprintf(Data, "Number of unread ACC and Gyro samples each: %d\n", unread_acc_and_gyro_samples);
	COM_uartPrint(Data);

	while (unread_acc_and_gyro_samples > 0)
	{
		uint16_t unread_acc_and_gyro_samples_cycle;
		if(unread_acc_and_gyro_samples >= SAMPLES_TO_READ)
		{
			unread_acc_and_gyro_samples_cycle = SAMPLES_TO_READ;
		}
		else
		{
			unread_acc_and_gyro_samples_cycle = unread_acc_and_gyro_samples;
		}

		lsm6dsl_fifo_raw_data_get(&Dev_Ctx, P_Data_Raw_Acc_Gy_Buf->u8bit, unread_acc_and_gyro_samples_cycle * ACC_AND_GYRO_SINGLE_FIFO_SAMPLE * FIFO_SAMPLE_TO_BYTE_RATIO);
		for (int i = 0; i < unread_acc_and_gyro_samples_cycle * ACC_AND_GYRO_SINGLE_FIFO_SAMPLE; i = i + ACC_AND_GYRO_SINGLE_FIFO_SAMPLE)
		{
			LSM6DSL_fifoAccAndGyroReadSingleSample(i);
		}
		LSM6DSL_fifoCalcAccGyroAvgAndPrint(unread_acc_and_gyro_samples_cycle);
		unread_acc_and_gyro_samples -= unread_acc_and_gyro_samples_cycle;
	}
}

void LSM6DSL_fifoAccAndGyroReadSingleSample(uint16_t SampleIndex)
{
	P_Angular_Rate_Dps_Sum[0] += P_Data_Raw_Acc_Gy_Buf->i16bit[SampleIndex];
	P_Angular_Rate_Dps_Sum[1] += P_Data_Raw_Acc_Gy_Buf->i16bit[SampleIndex + 1];
	P_Angular_Rate_Dps_Sum[2] += P_Data_Raw_Acc_Gy_Buf->i16bit[SampleIndex + 2];

	P_Acceleration_G_Sum[0] += P_Data_Raw_Acc_Gy_Buf->i16bit[SampleIndex + 3];
	P_Acceleration_G_Sum[1] += P_Data_Raw_Acc_Gy_Buf->i16bit[SampleIndex + 4];
	P_Acceleration_G_Sum[2] += P_Data_Raw_Acc_Gy_Buf->i16bit[SampleIndex + 5];
}

void LSM6DSL_fifoCalcAccGyroAvgAndPrint(uint16_t divider)
{
	sprintf(Data, "%d samples average:\n", divider);
	COM_uartPrint(Data);

	P_Acceleration_G_Sum[0] = lsm6dsl_from_fs8g_to_mg( P_Acceleration_G_Sum[0] / divider )/1000;
	P_Acceleration_G_Sum[1] = lsm6dsl_from_fs8g_to_mg( P_Acceleration_G_Sum[1] / divider )/1000;
	P_Acceleration_G_Sum[2] = lsm6dsl_from_fs8g_to_mg( P_Acceleration_G_Sum[2] / divider )/1000;

	sprintf(Data, "Acceleration [g]:  %4.2f\t%4.2f\t%4.2f\n",
			P_Acceleration_G_Sum[0], P_Acceleration_G_Sum[1], P_Acceleration_G_Sum[2]);

	COM_uartPrint(Data);

	P_Angular_Rate_Dps_Sum[0] = lsm6dsl_from_fs500dps_to_mdps( P_Angular_Rate_Dps_Sum[0] / divider )/1000;
	P_Angular_Rate_Dps_Sum[1] = lsm6dsl_from_fs500dps_to_mdps( P_Angular_Rate_Dps_Sum[1] / divider )/1000;
	P_Angular_Rate_Dps_Sum[2] = lsm6dsl_from_fs500dps_to_mdps( P_Angular_Rate_Dps_Sum[2] / divider )/1000;

	sprintf(Data, "Angular rate [dps]:%4.2f\t%4.2f\t%4.2f\n\n",
				P_Angular_Rate_Dps_Sum[0], P_Angular_Rate_Dps_Sum[1], P_Angular_Rate_Dps_Sum[2]);

	COM_uartPrint(Data);
}

void LSM6DSL_fifoSetFIFOMode(void)
{
	/* Set FIFO mode to FIFO */
	lsm6dsl_fifo_mode_t fifo_mode_t;
	lsm6dsl_fifo_mode_get(&Dev_Ctx, &fifo_mode_t);
	fifo_mode_t = LSM6DSL_FIFO_MODE;
	lsm6dsl_fifo_mode_set(&Dev_Ctx, fifo_mode_t);
}

void LSM6DSL_fifoSetBypassMode(void)
{
	/* Set FIFO mode to BYPASS */
	lsm6dsl_fifo_mode_t fifo_mode_t;
	lsm6dsl_fifo_mode_get(&Dev_Ctx, &fifo_mode_t);
	fifo_mode_t = LSM6DSL_BYPASS_MODE;
	lsm6dsl_fifo_mode_set(&Dev_Ctx, fifo_mode_t);

	//lsm6dsl_fifo_mode_set(&Dev_Ctx, LSM6DSL_BYPASS_MODE);
}

void LSM6DSL_fifoInterruptEnable(void)
{
	/* Set FIFO_FTH on INT1 */
	lsm6dsl_int1_route_t reg;
	lsm6dsl_pin_int1_route_get(&Dev_Ctx, &reg);
	//reg.int1_full_flag = PROPERTY_ENABLE;
	reg.int1_fth = PROPERTY_ENABLE;
	lsm6dsl_pin_int1_route_set(&Dev_Ctx, reg);
}

void LSM6DSL_fifoInterruptDisable(void)
{
	/* Set FIFO_FTH on INT1 */
	lsm6dsl_int1_route_t reg;
	lsm6dsl_pin_int1_route_get(&Dev_Ctx, &reg);
	reg.int1_fth = PROPERTY_DISABLE;
	lsm6dsl_pin_int1_route_set(&Dev_Ctx, reg);
}

void LSM6DSL_modesDisable(void)
{
	LSM6DSL_perSampleDisable();
	LSM6DSL_fifoDisable();
	Lsm6dsl_Mode = LSM6DSL_MODE_IDLE;
}

bool LSM6DSL_getInterruptFlag(void)
{
	return Interrup_Flag;
}

void LSM6DSL_setInterruptFlagOn(void)
{
	Interrup_Flag = true;
}

void LSM6DSL_setInterruptFlagOff(void)
{
	Interrup_Flag = false;
}
