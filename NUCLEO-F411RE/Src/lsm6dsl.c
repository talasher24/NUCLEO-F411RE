

#include <lsm6dsl.h>
#include "Buffer.h"


extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
extern void uart_print(char* token);
extern s_Buff s_uart_buffer;

stmdev_ctx_t dev_ctx;
char data[100];

int32_t LSM6DSL_Write(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
    HAL_I2C_Mem_Write(handle, LSM6DSL_I2C_ADD_H, Reg, I2C_MEMADD_SIZE_8BIT, Bufp, len, 1000);
    return 0;
}

int32_t LSM6DSL_Read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
	HAL_I2C_Mem_Read(handle, LSM6DSL_I2C_ADD_H, Reg, I2C_MEMADD_SIZE_8BIT, Bufp, len, 1000);
	return 0;
}

void LSM6DSL_Init (void)
{
	/* Initialize mems driver interface */
	dev_ctx.write_reg = LSM6DSL_Write;
	dev_ctx.read_reg = LSM6DSL_Read;
	dev_ctx.handle = &hi2c1;

	/* Check device ID */
	whoamI = 0;
	lsm6dsl_device_id_get(&dev_ctx, &whoamI);
	if ( whoamI != LSM6DSL_ID )
	{
		while(1); /* device not found */
	}

	/* Restore default configuration */
	lsm6dsl_reset_set(&dev_ctx, PROPERTY_ENABLE);
	do {
	lsm6dsl_reset_get(&dev_ctx, &rst);
	} while (rst);

	/* Enable Block Data Update */
	lsm6dsl_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

	/* Set Output Data Rate */
	lsm6dsl_xl_data_rate_set(&dev_ctx, LSM6DSL_XL_ODR_12Hz5);
	lsm6dsl_gy_data_rate_set(&dev_ctx, LSM6DSL_GY_ODR_12Hz5);

	/* Set full scale */
	lsm6dsl_xl_full_scale_set(&dev_ctx, LSM6DSL_8g);
	lsm6dsl_gy_full_scale_set(&dev_ctx, LSM6DSL_500dps);

	/* Accelerometer - analog filter */
	lsm6dsl_xl_filter_analog_set(&dev_ctx, LSM6DSL_XL_ANA_BW_400Hz);

	/* Accelerometer - LPF1 + LPF2 path */
	lsm6dsl_xl_lp2_bandwidth_set(&dev_ctx, LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);

	/* Gyroscope - filtering chain */
	lsm6dsl_gy_band_pass_set(&dev_ctx, LSM6DSL_HP_260mHz_LP1_STRONG);
}

void LSM6DSL_Per_Sample_Init(void)
{
	//LSM6DSL_ACC_Set_INT1_DRDY
	//LSM6DSL_GYRO_Set_INT1_DRDY

	lsm6dsl_pin_polarity_set(&dev_ctx, LSM6DSL_ACTIVE_LOW);

	lsm6dsl_data_ready_mode_set(&dev_ctx, LSM6DSL_DRDY_PULSED);

	lsm6dsl_int1_route_t reg;

	lsm6dsl_pin_int1_route_get(&dev_ctx, &reg);

	reg.int1_drdy_xl = PROPERTY_ENABLE;
	reg.int1_drdy_g = PROPERTY_ENABLE;

	lsm6dsl_pin_int1_route_set(&dev_ctx, reg);
}

void LSM6DSL_Per_Sample_Process (void)
{
	/*
	 * Read output only if new value is available
	 */
	lsm6dsl_reg_t reg;
	lsm6dsl_status_reg_get(&dev_ctx, &reg.status_reg);


	if (reg.status_reg.xlda)
	{
	  memset(data_raw_acceleration.u8bit, 0x00, 3*sizeof(int16_t));
	  lsm6dsl_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
	  acceleration_mg[0] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration.i16bit[0]);
	  acceleration_mg[1] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration.i16bit[1]);
	  acceleration_mg[2] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration.i16bit[2]);

	  sprintf(data, "Acceleration [mg]:  %4.2f\t%4.2f\t%4.2f\n",
	  			  acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);

	  uart_print(data);
	  //BufferInit((uint8_t*)data);
	  //HAL_UART_Transmit( &huart2, s_uart_buffer._p_tx_buffer, strlen( (char*)s_uart_buffer._p_tx_buffer ), 1000 );
	}
	if (reg.status_reg.gda)
	{
	  memset(data_raw_angular_rate.u8bit, 0x00, 3*sizeof(int16_t));
	  lsm6dsl_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);
	  angular_rate_mdps[0] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate.i16bit[0]);
	  angular_rate_mdps[1] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate.i16bit[1]);
	  angular_rate_mdps[2] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate.i16bit[2]);

	  sprintf(data, "Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\n\n",
			  angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);

	  uart_print(data);
	  //BufferInit((uint8_t*)data);
	  //HAL_UART_Transmit( &huart2, s_uart_buffer._p_tx_buffer, strlen( (char*)s_uart_buffer._p_tx_buffer ), 1000 );
	}
}

void LSM6DSL_Per_Sample_Disable (void)
{
	lsm6dsl_int1_route_t reg;

	lsm6dsl_pin_int1_route_get(&dev_ctx, &reg);

	reg.int1_drdy_xl = PROPERTY_DISABLE;
	reg.int1_drdy_g = PROPERTY_DISABLE;

	lsm6dsl_pin_int1_route_set(&dev_ctx, reg);
}

void LSM6DSL_FIFO_Init(void)
{
	lsm6dsl_pin_polarity_set(&dev_ctx, LSM6DSL_ACTIVE_LOW);

	/* Set acc&gyro FIFO decimation */
	lsm6dsl_dec_fifo_xl_t valXl;
	lsm6dsl_dec_fifo_gyro_t valGy;

	lsm6dsl_fifo_xl_batch_get(&dev_ctx, &valXl);
	valXl = LSM6DSL_FIFO_XL_NO_DEC;
	lsm6dsl_fifo_xl_batch_set(&dev_ctx, valXl);

	lsm6dsl_fifo_gy_batch_get(&dev_ctx, &valGy);
	valGy = LSM6DSL_FIFO_GY_NO_DEC;
	lsm6dsl_fifo_gy_batch_set(&dev_ctx, valGy);

	/* Set FIFO ODR value */
	lsm6dsl_odr_fifo_t valOdr;
	lsm6dsl_fifo_data_rate_get(&dev_ctx, &valOdr);
	valOdr = LSM6DSL_FIFO_12Hz5;
	lsm6dsl_fifo_data_rate_set(&dev_ctx, valOdr);

	LSM6DSL_FIFO_Interrupt_Enable();

	/* Set FIFO watermark */
	uint16_t valWtm;
	lsm6dsl_fifo_watermark_get(&dev_ctx, &valWtm);
	valWtm = ACC_AND_GYRO_FIFO_WATERMARK;
	lsm6dsl_fifo_watermark_set(&dev_ctx, valWtm);

	/* Set FIFO depth to be limited to watermark threshold level  */
	/*uint8_t valStopOnWtm;
	lsm6dsl_fifo_stop_on_wtm_get(&dev_ctx, &valStopOnWtm);
	valStopOnWtm = PROPERTY_ENABLE;
	lsm6dsl_fifo_stop_on_wtm_set(&dev_ctx, valStopOnWtm);*/

	LSM6DSL_FIFO_Set_FIFO_Mode();
}

void LSM6DSL_FIFO_Process(void)
{
	LSM6DSL_FIFO_Read_All_Data();
}

void LSM6DSL_FIFO_Disable(void)
{
	LSM6DSL_FIFO_Interrupt_Disable();
	LSM6DSL_FIFO_Set_Bypass_Mode();
}

void LSM6DSL_FIFO_Read_All_Data(void)
{
	uint16_t unread_int16_fifo_samples = 0;

	for(int i = 0; i < 3; i++)
	{
		acceleration_g_Sum[i] = 0;
		angular_rate_dps_Sum[i] = 0;
	}

	/* Get num of unread FIFO samples before reading data */
	lsm6dsl_fifo_data_level_get(&dev_ctx, &unread_int16_fifo_samples);

	uint16_t unread_acc_and_gyro_samples = unread_int16_fifo_samples / ACC_AND_GYRO_SINGLE_FIFO_SAMPLE;
	sprintf(data, "Number of unread ACC and Gyro samples each: %d\n", unread_acc_and_gyro_samples);
	uart_print(data);

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

		lsm6dsl_fifo_raw_data_get(&dev_ctx, data_raw_acc_gy_Buf->u8bit, unread_acc_and_gyro_samples_cycle * ACC_AND_GYRO_SINGLE_FIFO_SAMPLE * FIFO_SAMPLE_TO_BYTE_RATIO);
		for (int i = 0; i < unread_acc_and_gyro_samples_cycle * ACC_AND_GYRO_SINGLE_FIFO_SAMPLE; i = i + ACC_AND_GYRO_SINGLE_FIFO_SAMPLE)
		{
			LSM6DSL_FIFO_Acc_And_Gyro_Read_Single_SAMPLE(i);
		}
		LSM6DSL_FIFO_Calc_Acc_Gyro_Avg_And_Print(unread_acc_and_gyro_samples_cycle);
		unread_acc_and_gyro_samples -= unread_acc_and_gyro_samples_cycle;


		/*if ((unread_int16_fifo_samples/ACC_AND_GYRO_SINGLE_FIFO_SAMPLE) >= SAMPLES_TO_READ) 		// reading 10 GYRO & ACC samples each
		{
			lsm6dsl_fifo_raw_data_get(&dev_ctx, data_raw_acc_gy_Buf->u8bit, ACC_GYRO_BUF_BYTES_SIZE);
			for (int i = 0; i < ACC_AND_GYRO_FIFO_WATERMARK; i = i + ACC_AND_GYRO_SINGLE_FIFO_SAMPLE)
			{
				LSM6DSL_FIFO_Acc_And_Gyro_Read_Single_SAMPLE(i);
			}
			LSM6DSL_FIFO_Calc_Acc_Gyro_Avg_And_Print(SAMPLES_TO_READ);
			unread_int16_fifo_samples -= ACC_AND_GYRO_FIFO_WATERMARK;
		}
		else 																				//reading less than 10 GYRO & ACC samples each
		{ 	//should be an if condition to ensure that unread_int16_fifo_samples is grater than 6
			lsm6dsl_fifo_raw_data_get(&dev_ctx, data_raw_acc_gy_Buf->u8bit, unread_int16_fifo_samples * FIFO_SAMPLE_TO_BYTE_RATIO);
			for (int i = 0; i < unread_int16_fifo_samples; i = i + ACC_AND_GYRO_SINGLE_FIFO_SAMPLE)
			{
				LSM6DSL_FIFO_Acc_And_Gyro_Read_Single_SAMPLE(i);
			}
			LSM6DSL_FIFO_Calc_Acc_Gyro_Avg_And_Print(unread_int16_fifo_samples/ACC_AND_GYRO_SINGLE_FIFO_SAMPLE);
			unread_int16_fifo_samples = 0;
		}*/
	}
}
void LSM6DSL_FIFO_Acc_And_Gyro_Read_Single_SAMPLE(uint16_t SampleIndex)
{
	angular_rate_dps_Sum[0] += data_raw_acc_gy_Buf->i16bit[SampleIndex];
	angular_rate_dps_Sum[1] += data_raw_acc_gy_Buf->i16bit[SampleIndex + 1];
	angular_rate_dps_Sum[2] += data_raw_acc_gy_Buf->i16bit[SampleIndex + 2];

	acceleration_g_Sum[0] += data_raw_acc_gy_Buf->i16bit[SampleIndex + 3];
	acceleration_g_Sum[1] += data_raw_acc_gy_Buf->i16bit[SampleIndex + 4];
	acceleration_g_Sum[2] += data_raw_acc_gy_Buf->i16bit[SampleIndex + 5];
}

void LSM6DSL_FIFO_Calc_Acc_Gyro_Avg_And_Print(uint16_t divider)
{
	sprintf(data, "%d samples average:\n", divider);
	uart_print(data);

	acceleration_g_Sum[0] = lsm6dsl_from_fs8g_to_mg( acceleration_g_Sum[0] / divider )/1000;
	acceleration_g_Sum[1] = lsm6dsl_from_fs8g_to_mg( acceleration_g_Sum[1] / divider )/1000;
	acceleration_g_Sum[2] = lsm6dsl_from_fs8g_to_mg( acceleration_g_Sum[2] / divider )/1000;

	sprintf(data, "Acceleration [g]:  %4.2f\t%4.2f\t%4.2f\n",
			acceleration_g_Sum[0], acceleration_g_Sum[1], acceleration_g_Sum[2]);

	uart_print(data);

	angular_rate_dps_Sum[0] = lsm6dsl_from_fs500dps_to_mdps( angular_rate_dps_Sum[0] / divider )/1000;
	angular_rate_dps_Sum[1] = lsm6dsl_from_fs500dps_to_mdps( angular_rate_dps_Sum[1] / divider )/1000;
	angular_rate_dps_Sum[2] = lsm6dsl_from_fs500dps_to_mdps( angular_rate_dps_Sum[2] / divider )/1000;

	sprintf(data, "Angular rate [dps]:%4.2f\t%4.2f\t%4.2f\n\n",
				angular_rate_dps_Sum[0], angular_rate_dps_Sum[1], angular_rate_dps_Sum[2]);

	uart_print(data);
}

void LSM6DSL_FIFO_Set_FIFO_Mode(void)
{
	/* Set FIFO mode to FIFO */
	lsm6dsl_fifo_mode_t fifo_mode_t;
	lsm6dsl_fifo_mode_get(&dev_ctx, &fifo_mode_t);
	fifo_mode_t = LSM6DSL_FIFO_MODE;
	lsm6dsl_fifo_mode_set(&dev_ctx, fifo_mode_t);

	//lsm6dsl_fifo_mode_set(&dev_ctx, LSM6DSL_FIFO_MODE);
}

void LSM6DSL_FIFO_Set_Bypass_Mode(void)
{
	/* Set FIFO mode to BYPASS */
	lsm6dsl_fifo_mode_t fifo_mode_t;
	lsm6dsl_fifo_mode_get(&dev_ctx, &fifo_mode_t);
	fifo_mode_t = LSM6DSL_BYPASS_MODE;
	lsm6dsl_fifo_mode_set(&dev_ctx, fifo_mode_t);

	//lsm6dsl_fifo_mode_set(&dev_ctx, LSM6DSL_BYPASS_MODE);
}

void LSM6DSL_FIFO_Interrupt_Enable(void)
{
	/* Set FIFO_FTH on INT1 */
	lsm6dsl_int1_route_t reg;
	lsm6dsl_pin_int1_route_get(&dev_ctx, &reg);
	//reg.int1_full_flag = PROPERTY_ENABLE;
	reg.int1_fth = PROPERTY_ENABLE;
	lsm6dsl_pin_int1_route_set(&dev_ctx, reg);
}

void LSM6DSL_FIFO_Interrupt_Disable(void)
{
	/* Set FIFO_FTH on INT1 */
	lsm6dsl_int1_route_t reg;
	lsm6dsl_pin_int1_route_get(&dev_ctx, &reg);
	reg.int1_fth = PROPERTY_DISABLE;
	lsm6dsl_pin_int1_route_set(&dev_ctx, reg);
}

