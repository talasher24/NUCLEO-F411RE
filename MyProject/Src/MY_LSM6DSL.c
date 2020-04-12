

#include "MY_LSM6DSL.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
extern void uart_print(char* token);


stmdev_ctx_t dev_ctx;

static int32_t lsm_write(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
    HAL_I2C_Mem_Write(handle, LSM6DSL_I2C_ADD_H, Reg,
                      I2C_MEMADD_SIZE_8BIT, Bufp, len, 1000);
    return 0;
}

static int32_t lsm_read(void *handle, uint8_t Reg, uint8_t *Bufp,
                             uint16_t len)
{
	HAL_I2C_Mem_Read(handle, LSM6DSL_I2C_ADD_H, Reg,
                       I2C_MEMADD_SIZE_8BIT, Bufp, len, 1000);
	return 0;
}

void lsm_init (void)
{
	/*
	*  Initialize mems driver interface
	*/
	dev_ctx.write_reg = lsm_write;
	dev_ctx.read_reg = lsm_read;
	dev_ctx.handle = &hi2c1;
	/*
	*  Check device ID
	*/
	whoamI = 0;
	lsm6dsl_device_id_get(&dev_ctx, &whoamI);
	if ( whoamI != LSM6DSL_ID )
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		while(1); /*manage here device not found */
	}

	/*
	*  Restore default configuration
	*/
	lsm6dsl_reset_set(&dev_ctx, PROPERTY_ENABLE);
	do {
	lsm6dsl_reset_get(&dev_ctx, &rst);
	} while (rst);
	/*
	*  Enable Block Data Update
	*/
	lsm6dsl_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
	/*
	* Set Output Data Rate
	*/
	lsm6dsl_xl_data_rate_set(&dev_ctx, LSM6DSL_XL_ODR_12Hz5);
	lsm6dsl_gy_data_rate_set(&dev_ctx, LSM6DSL_GY_ODR_12Hz5);
	/*
	* Set full scale
	*/
	lsm6dsl_xl_full_scale_set(&dev_ctx, LSM6DSL_8g);
	lsm6dsl_gy_full_scale_set(&dev_ctx, LSM6DSL_500dps);

	/*
	* Configure filtering chain(No aux interface)
	*/
	/* Accelerometer - analog filter */
	lsm6dsl_xl_filter_analog_set(&dev_ctx, LSM6DSL_XL_ANA_BW_400Hz);

	/* Accelerometer - LPF1 path ( LPF2 not used )*/
	//lsm6dsl_xl_lp1_bandwidth_set(&dev_ctx, LSM6DSL_XL_LP1_ODR_DIV_4);

	/* Accelerometer - LPF1 + LPF2 path */
	lsm6dsl_xl_lp2_bandwidth_set(&dev_ctx, LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);

	/* Accelerometer - High Pass / Slope path */
	//lsm6dsl_xl_reference_mode_set(&dev_ctx, PROPERTY_DISABLE);
	//lsm6dsl_xl_hp_bandwidth_set(&dev_ctx, LSM6DSL_XL_HP_ODR_DIV_100);

	/* Gyroscope - filtering chain */
	lsm6dsl_gy_band_pass_set(&dev_ctx, LSM6DSL_HP_260mHz_LP1_STRONG);

	/*
	* Read samples in polling mode (no int)
	*/
}

void lsm_callback (void)
{
	/*
	 * Read output only if new value is available
	 */
	lsm6dsl_reg_t reg;
	lsm6dsl_status_reg_get(&dev_ctx, &reg.status_reg);

	if (reg.status_reg.xlda)
	{
	  /* Read magnetic field data */
	  memset(data_raw_acceleration.u8bit, 0x00, 3*sizeof(int16_t));
	  lsm6dsl_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
	  acceleration_mg[0] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration.i16bit[0]);
	  acceleration_mg[1] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration.i16bit[1]);
	  acceleration_mg[2] = lsm6dsl_from_fs8g_to_mg( data_raw_acceleration.i16bit[2]);

	  sprintf((char*)tx_buffer, "Acceleration [mg]:  %4.2f\t%4.2f\t%4.2f\n",
			  acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);

	  HAL_UART_Transmit( &huart2, tx_buffer, strlen( (char const*)tx_buffer ), 1000 );
	}
	if (reg.status_reg.gda)
	{
	  /* Read magnetic field data */
	  memset(data_raw_angular_rate.u8bit, 0x00, 3*sizeof(int16_t));
	  lsm6dsl_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);
	  angular_rate_mdps[0] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate.i16bit[0]);
	  angular_rate_mdps[1] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate.i16bit[1]);
	  angular_rate_mdps[2] = lsm6dsl_from_fs500dps_to_mdps(data_raw_angular_rate.i16bit[2]);

	  sprintf((char*)tx_buffer, "Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\n",
			  angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);

	  HAL_UART_Transmit( &huart2, tx_buffer, strlen( (char const*)tx_buffer ), 1000 );
	}
}