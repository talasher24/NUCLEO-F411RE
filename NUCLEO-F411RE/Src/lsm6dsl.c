

#include <lsm6dsl.h>
#include "Buffer.h"


extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
extern void uart_print(char* token);
extern s_Buff s_uart_buffer;

stmdev_ctx_t dev_ctx;
char data[100];

int32_t LSM6DSL_WRITE(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
    HAL_I2C_Mem_Write(handle, LSM6DSL_I2C_ADD_H, Reg, I2C_MEMADD_SIZE_8BIT, Bufp, len, 1000);
    return 0;
}

int32_t LSM6DSL_READ(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
	HAL_I2C_Mem_Read(handle, LSM6DSL_I2C_ADD_H, Reg, I2C_MEMADD_SIZE_8BIT, Bufp, len, 1000);
	return 0;
}

void LSM6DSL_INIT (void)
{
	/* Initialize mems driver interface */
	dev_ctx.write_reg = LSM6DSL_WRITE;
	dev_ctx.read_reg = LSM6DSL_READ;
	dev_ctx.handle = &hi2c1;

	/* Check device ID */
	whoamI = 0;
	lsm6dsl_device_id_get(&dev_ctx, &whoamI);
	if ( whoamI != LSM6DSL_ID )
	{
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
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

void LSM6DSL_PER_SAMPLE_Init(void)
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

void LSM6DSL_PER_SAMPLE_PROCESS (void)
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

void LSM6DSL_PER_SAMPLE_DISABLE (void)
{
	lsm6dsl_int1_route_t reg;

	lsm6dsl_pin_int1_route_get(&dev_ctx, &reg);

	reg.int1_drdy_xl = PROPERTY_DISABLE;
	reg.int1_drdy_g = PROPERTY_DISABLE;

	lsm6dsl_pin_int1_route_set(&dev_ctx, reg);
}

void LSM6DSL_FIFOMode_Init(void)
{
	lsm6dsl_pin_polarity_set(&dev_ctx, LSM6DSL_ACTIVE_LOW);

	lsm6dsl_data_ready_mode_set(&dev_ctx, LSM6DSL_DRDY_PULSED);

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

	/* Set FIFO_FULL on INT1 */
	lsm6dsl_int1_route_t reg;
	lsm6dsl_pin_int1_route_get(&dev_ctx, &reg);
	reg.int1_full_flag = PROPERTY_ENABLE;
	//reg.int1_fth = PROPERTY_ENABLE;
	lsm6dsl_pin_int1_route_set(&dev_ctx, reg);

	/* Set FIFO watermark */
	uint16_t valWtm;
	lsm6dsl_fifo_watermark_get(&dev_ctx, &valWtm);
	valWtm = FIFO_WATERMARK_ACC_GY;
	lsm6dsl_fifo_watermark_set(&dev_ctx, valWtm);

	/* Set FIFO depth to be limited to watermark threshold level  */
	uint8_t valStopOnWtm;
	lsm6dsl_fifo_stop_on_wtm_get(&dev_ctx, &valStopOnWtm);
	valStopOnWtm = PROPERTY_ENABLE;
	lsm6dsl_fifo_stop_on_wtm_set(&dev_ctx, valStopOnWtm);

	/* Set FIFO trigger */
	/*lsm6dsl_trigger_fifo_t trigger_fifo_t;
	lsm6dsl_fifo_write_trigger_get(&dev_ctx, &trigger_fifo_t);
	trigger_fifo_t = LSM6DSL_TRG_XL_GY_DRDY;
	lsm6dsl_fifo_write_trigger_set(&dev_ctx, trigger_fifo_t);*/

	/*lsm6dsl_den_xl_en_t den_xl_en_t;
	lsm6dsl_den_enable_get(&dev_ctx, &den_xl_en_t);
	den_xl_en_t = LSM6DSL_STAMP_IN_GY_XL_DATA;
	lsm6dsl_den_enable_set(&dev_ctx, den_xl_en_t);*/

	/* Set FIFO mode to FIFO */
	/*lsm6dsl_fifo_mode_t fifo_mode_t;
	lsm6dsl_fifo_mode_get(&dev_ctx, &fifo_mode_t);
	fifo_mode_t = LSM6DSL_FIFO_MODE;
	lsm6dsl_fifo_mode_set(&dev_ctx, fifo_mode_t);*/

	LSM6DSL_FIFO_Set_FIFO_Mode();
}
void LSM6DSL_FIFOMode_Process(void)
{
	LSM6DSL_Read_All_FIFO_Data();

	LSM6DSL_FIFO_Set_Bypass_Mode();

	LSM6DSL_FIFO_Set_FIFO_Mode();
}

void LSM6DSL_Read_All_FIFO_Data(void)
{
	uint16_t samples_to_read = 0;

	for(int i = 0; i < 3; i++)
	{
		acceleration_g_Sum[i] = 0;
		angular_rate_dps_Sum[i] = 0;
	}

	/* Get num of unread FIFO samples before reading data */
	lsm6dsl_fifo_data_level_get(&dev_ctx, &samples_to_read);

	samples_to_read /= 6U;

	if(samples_to_read == SAMPLES_TO_READ)
	{
		for (int i = 0; i < samples_to_read; i++)
		{
			LSM6DSL_Read_Single_FIFO_SAMPLE(i);
		}


		uart_print("10 samples average:\n");

		acceleration_g_Sum[0] = lsm6dsl_from_fs8g_to_mg( acceleration_g_Sum[0] / samples_to_read )/1000;
		acceleration_g_Sum[1] = lsm6dsl_from_fs8g_to_mg( acceleration_g_Sum[1] / samples_to_read )/1000;
		acceleration_g_Sum[2] = lsm6dsl_from_fs8g_to_mg( acceleration_g_Sum[2] / samples_to_read )/1000;

		sprintf(data, "Acceleration [g]:  %4.2f\t%4.2f\t%4.2f\n",
				acceleration_g_Sum[0], acceleration_g_Sum[1], acceleration_g_Sum[2]);

		uart_print(data);

		angular_rate_dps_Sum[0] = lsm6dsl_from_fs500dps_to_mdps( angular_rate_dps_Sum[0] / samples_to_read )/1000;
		angular_rate_dps_Sum[1] = lsm6dsl_from_fs500dps_to_mdps( angular_rate_dps_Sum[1] / samples_to_read )/1000;
		angular_rate_dps_Sum[2] = lsm6dsl_from_fs500dps_to_mdps( angular_rate_dps_Sum[2] / samples_to_read )/1000;

		sprintf(data, "Angular rate [dps]:%4.2f\t%4.2f\t%4.2f\n\n",
					angular_rate_dps_Sum[0], angular_rate_dps_Sum[1], angular_rate_dps_Sum[2]);

		uart_print(data);
	}
}
void LSM6DSL_Read_Single_FIFO_SAMPLE(uint16_t SampleIndex)
{

	lsm6dsl_fifo_raw_data_get(&dev_ctx, data_raw_angular_rate_Buf[SampleIndex].u8bit, 6);
	angular_rate_dps_Sum[0] += data_raw_angular_rate_Buf[SampleIndex].i16bit[0];
	angular_rate_dps_Sum[1] += data_raw_angular_rate_Buf[SampleIndex].i16bit[1];
	angular_rate_dps_Sum[2] += data_raw_angular_rate_Buf[SampleIndex].i16bit[2];


	lsm6dsl_fifo_raw_data_get(&dev_ctx, data_raw_acceleration_Buf[SampleIndex].u8bit, 6);
	acceleration_g_Sum[0] += data_raw_acceleration_Buf[SampleIndex].i16bit[0];
	acceleration_g_Sum[1] += data_raw_acceleration_Buf[SampleIndex].i16bit[1];
	acceleration_g_Sum[2] += data_raw_acceleration_Buf[SampleIndex].i16bit[2];


	/*lsm6dsl_fifo_raw_data_get(&dev_ctx, data_raw_angular_rate_Buf[SampleIndex].u8bit, 6);
	angular_rate_mdps_Buf[SampleIndex][0] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate_Buf[SampleIndex].i16bit[0]);
	angular_rate_mdps_Buf[SampleIndex][1] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate_Buf[SampleIndex].i16bit[1]);
	angular_rate_mdps_Buf[SampleIndex][2] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate_Buf[SampleIndex].i16bit[2]);

	sprintf(data, "Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\n",
			angular_rate_mdps_Buf[SampleIndex][0], angular_rate_mdps_Buf[SampleIndex][1], angular_rate_mdps_Buf[SampleIndex][2]);

	uart_print(data);

	lsm6dsl_fifo_raw_data_get(&dev_ctx, data_raw_acceleration_Buf[SampleIndex].u8bit, 6);
	acceleration_mg_Buf[SampleIndex][0] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration_Buf[SampleIndex].i16bit[0]);
	acceleration_mg_Buf[SampleIndex][1] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration_Buf[SampleIndex].i16bit[1]);
	acceleration_mg_Buf[SampleIndex][2] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration_Buf[SampleIndex].i16bit[2]);

	sprintf(data, "Acceleration [mg]:  %4.2f\t%4.2f\t%4.2f\n\n",
			acceleration_mg_Buf[SampleIndex][0], acceleration_mg_Buf[SampleIndex][1], acceleration_mg_Buf[SampleIndex][2]);

	uart_print(data);*/

	/*lsm6dsl_fifo_raw_data_get(&dev_ctx, data_raw_angular_rate.u8bit, 6);
	angular_rate_mdps[0] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate.i16bit[0]);
	angular_rate_mdps[1] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate.i16bit[1]);
	angular_rate_mdps[2] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate.i16bit[2]);

	sprintf(data, "Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\n",
				  angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);

	uart_print(data);

	lsm6dsl_fifo_raw_data_get(&dev_ctx, data_raw_acceleration.u8bit, 6);
	acceleration_mg[0] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration.i16bit[0]);
	acceleration_mg[1] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration.i16bit[1]);
	acceleration_mg[2] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration.i16bit[2]);

	sprintf(data, "Acceleration [mg]:  %4.2f\t%4.2f\t%4.2f\n\n",
					  acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);

	uart_print(data);*/
}

void LSM6DSL_FIFO_Set_FIFO_Mode(void)
{
	/* Set FIFO mode to FIFO */
	lsm6dsl_fifo_mode_set(&dev_ctx, LSM6DSL_FIFO_MODE);
}

void LSM6DSL_FIFO_Set_Bypass_Mode(void)
{
	/* Set FIFO mode to BYPASS */
	lsm6dsl_fifo_mode_set(&dev_ctx, LSM6DSL_BYPASS_MODE);
}

void LSM6DSL_FIFOMode_DISABLE(void)
{
	LSM6DSL_FIFO_Set_Bypass_Mode();

	lsm6dsl_int1_route_t reg;
	lsm6dsl_pin_int1_route_get(&dev_ctx, &reg);
	reg.int1_full_flag = PROPERTY_DISABLE;
	//reg.int1_fth = PROPERTY_ENABLE;
	lsm6dsl_pin_int1_route_set(&dev_ctx, reg);
}

