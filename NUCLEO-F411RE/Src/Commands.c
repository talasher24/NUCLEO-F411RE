/*
 * Commands.c
 *
 *  Created on: Apr 5, 2020
 *      Author: ADMIN
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include <Flash.h>
#include <lsm6dsl.h>
#include "Commands.h"
#include "Buffer.h"
#include "Types.h"
#include "Debug.h"


 extern TIM_HandleTypeDef htim3;
 extern CRC_HandleTypeDef hcrc;
 extern RTC_HandleTypeDef hrtc;

 extern void MX_RTC_Init(void);
 extern void uart_print(char* token);
 extern s_Buff s_uart_buffer;
 extern assert_struct s_assert_struct;
 extern stmdev_ctx_t dev_ctx;



void ping_callBack(char* token)
{
	uart_print(token);
}

void get_version_callback(char* token)
{
	uart_print(VERSION);
}

void pwm_start_callback(char* token)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void pwm_stop_callback(char* token)
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

void pwm_dc_callback(char* token)
{
	token = strtok(NULL, " ");
	if(strlen(token) <= 0)
	{
		return;
	}
	int32_t dc = atoi(token);
	if (dc > 100) dc = 100;
	if (dc < 0)   dc = 0;
	htim3.Instance->CCR1 = dc;
}

void crc_whole_flash_calc_callback(char* token)
{
	char temp [9];
	uint32_t crcFlashResult;
	uint32_t flashSize = 0x20000;
	uint32_t *p_flash_start_address = (uint32_t *) FLASH_START_ADDRESS;

	crcFlashResult = HAL_CRC_Calculate(&hcrc, p_flash_start_address, flashSize);

	sprintf(temp, "%x\n", (unsigned int)crcFlashResult);
	uart_print(temp);
}

void iwdg_test_callback(char* token)
{
	uart_print(OK);
	while(1);
}

void flash_lock_callback(char* token)
{

	/*RDP protects the whole internal flash from reading from outside, via debugger interface.
	 *It does not prevent one part of code to read another part, or even rewrite.
	 *PCROP allows you to use debugger to debug your code,
	 *but protects reading and rewriting the "secret" part.*/

	FLASH_OBProgramInitTypeDef obConfig;
	HAL_FLASHEx_OBGetConfig(&obConfig);

	if (obConfig.RDPLevel == OB_RDP_LEVEL_0) {
		obConfig.RDPLevel = OB_RDP_LEVEL_1;
		obConfig.OptionType = OPTIONBYTE_RDP;

		if (HAL_FLASH_Unlock() != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_FLASH_OB_Unlock() != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_FLASHEx_OBProgram(&obConfig) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_FLASH_OB_Launch() != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_FLASH_OB_Lock() != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_FLASH_Lock() != HAL_OK)
		{
			Error_Handler();
		}

		/*
		//DEBUG - use for writing data to flash
		uint32_t sector_7_addr       = 0x08060000;		//Sector 7 address
		MY_FLASH_SetSectorAddrs(7, sector_7_addr);
		uint32_t myTestWrite[1] = {0xDEADBEEF}; 		//0xFFFFFFFF, 0xDEADBEEF
		MY_FLASH_WriteN(131072-4, myTestWrite2, 1, DATA_TYPE_32);
		*/
	}
}

void set_SN_callback(char* token)
{
	/*if (WRP_sector_disable() != HAL_OK)
	{
		Error_Handler();
	}*/
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	MY_FLASH_SetSectorAddrs(7, sector_7_addr);
	uint32_t myTestWrite[1] = {0xDEADBEEF}; //0xFFFFFFFF, 0xDEADBEEF
	MY_FLASH_WriteN(0, myTestWrite, 1, DATA_TYPE_32);

	/*if (WRP_sector_enable() != HAL_OK)
	{
		Error_Handler();
	}*/
}

void get_SN_callback(char* token)
{
	char temp [9];
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	MY_FLASH_SetSectorAddrs(7, sector_7_addr);
	uint32_t myTestRead[1];
	MY_FLASH_ReadN(0, myTestRead, 1, DATA_TYPE_32);

	sprintf(temp, "%x\n", (unsigned int)myTestRead[0]);
	uart_print(temp);
}

void start_tick_callback(char* token)
{
	RTC_AlarmTypeDef sAlarm = {0};
	  /** Enable the Alarm A
	  */
	  sAlarm.AlarmTime.Hours = 0;
	  sAlarm.AlarmTime.Minutes = 0;
	  sAlarm.AlarmTime.Seconds = 1;
	  sAlarm.AlarmTime.SubSeconds = 0;
	  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	  sAlarm.AlarmMask = RTC_ALARMMASK_ALL;
	  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	  sAlarm.AlarmDateWeekDay = 1;
	  sAlarm.Alarm = RTC_ALARM_A;
	  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void stop_tick_callback(char* token)
{
	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
}

void assert_0_callback(char* token)
{
	assert_param(0);
}

void clear_assert_flag_callback(char* token)
{
	s_assert_struct.flag = ASSERT_FLAG_OFF;
}

void lsm6dsl_per_sample_enable_callback(char* token)
{
	LSM6DSL_Per_Sample_Init();
}

void lsm6dsl_fifo_enable_callback(char* token)
{
	LSM6DSL_FIFO_Init();
}

void lsm6dsl_disable_callback(char* token)
{
	LSM6DSL_Mode_Disable();
}

void enter_stop_mode_callback(char* token)
{
	enter_stop_mode();
}

void enter_standby_mode_callback(char* token)
{
	enter_standby_mode();
}

