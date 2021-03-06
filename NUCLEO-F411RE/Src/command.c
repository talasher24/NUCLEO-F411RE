/*
 * command.c
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

/******************************************************************************
* Includes
*******************************************************************************/

#include "command.h"
#include "stm32f4xx_hal.h"
#include "com.h"
#include "flash.h"
#include "lsm6dsl.h"
#include "types.h"
#include "system_debug.h"

#include "crc.h"
#include "rtc.h"
#include "tim.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

#define NUM_OF_COMMANDS 19

#define PING_COMMAND_NAME "ping"

#define GET_VERSION_COMMAND_NAME "get_version"

#define PWM_START_COMMAND_NAME "pwm_start"

#define PWM_STOP_COMMAND_NAME "pwm_stop"

#define PWM_DC_COMMAND_NAME "pwm_dc"

#define CRC_WHOLE_FLASH_CALC "crc_whole_flash_calc"

#define IWDG_TEST "iwdg_test"

#define FLASH_LOCK "flash_lock"

#define set_SN "set_SN"

#define get_SN "get_SN"

#define START_TICK "start_tick"

#define STOP_TICK "stop_tick"

#define ASSERT_0 "assert_0"

#define CLEAR_ASSERT_FLAG "clear_assert_flag"

#define LSM6DSL_PER_SAMPLE_ENABLE "lsm6dsl_per_sample_enable"

#define LSM6DSL_FIFO_ENABLE "lsm6dsl_fifo_enable"

#define LSM6DSL_DISABLE "lsm6dsl_disable"

#define ENTER_STOP_MODE "enter_stop_mode"

#define ENTER_STANDBY_MODE "enter_standby_mode"

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

 typedef struct {
	char* 	name;
	uint8_t size;
	void 	(*func_ptr)(char*);
 } command_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

 const command_t Commands [NUM_OF_COMMANDS] = {
		{PING_COMMAND_NAME, 			sizeof(PING_COMMAND_NAME), 			COMMAND_pingCallback						},
 		{GET_VERSION_COMMAND_NAME, 		sizeof(GET_VERSION_COMMAND_NAME), 	COMMAND_getVersionCallback					},
 		{PWM_START_COMMAND_NAME, 		sizeof(PWM_START_COMMAND_NAME), 	COMMAND_pwmStartCallback					},
 		{PWM_STOP_COMMAND_NAME, 		sizeof(PWM_STOP_COMMAND_NAME), 		COMMAND_pwmStopCallback						},
 		{PWM_DC_COMMAND_NAME, 			sizeof(PWM_DC_COMMAND_NAME), 		COMMAND_pwmDcCallback						},
 		{CRC_WHOLE_FLASH_CALC, 			sizeof(CRC_WHOLE_FLASH_CALC), 		COMMAND_crcWholeFlashCalcCallback			},
 		{IWDG_TEST, 					sizeof(IWDG_TEST), 					COMMAND_iwdgTestCallback					},
 		{FLASH_LOCK, 					sizeof(FLASH_LOCK), 				COMMAND_flashLockCallback					},
 		{set_SN, 						sizeof(set_SN), 					COMMAND_setSNCallback						},
 		{get_SN, 						sizeof(get_SN), 					COMMAND_getSNCallback						},
 		{START_TICK, 					sizeof(START_TICK), 				COMMAND_startTickCallback					},
 		{STOP_TICK, 					sizeof(STOP_TICK), 					COMMAND_stopTickCallback					},
 		{ASSERT_0, 						sizeof(ASSERT_0), 					COMMAND_assert0Callback						},
 		{CLEAR_ASSERT_FLAG, 			sizeof(CLEAR_ASSERT_FLAG), 			COMMAND_clearAssertFlagCallback				},
 		{LSM6DSL_PER_SAMPLE_ENABLE, 	sizeof(LSM6DSL_PER_SAMPLE_ENABLE), 	COMMAND_lsm6dslPerSampleEnableCallback		},
 		{LSM6DSL_FIFO_ENABLE, 			sizeof(LSM6DSL_FIFO_ENABLE), 		COMMAND_lsm6dslFifoEnableCallback			},
 		{LSM6DSL_DISABLE, 				sizeof(LSM6DSL_DISABLE), 			COMMAND_lsm6dslDisableCallback				},
 		{ENTER_STOP_MODE, 				sizeof(ENTER_STOP_MODE), 			COMMAND_enterStopModeCallback				},
 		{ENTER_STANDBY_MODE, 			sizeof(ENTER_STANDBY_MODE), 		COMMAND_enterStandbyModeCallback			}
 };

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

void COMMAND_findAndExecuteCommand (char* token)
{
	for (uint8_t i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if (strncmp(token, Commands[i].name, Commands[i].size)==0)
		{
			COM_uartPrint(OK);
			Commands[i].func_ptr(token);
			return;
		}
	}
}

void COMMAND_pingCallback(char* token)
{
	COM_uartPrint(token);
}

void COMMAND_getVersionCallback(char* token)
{
	COM_uartPrint(VERSION);
}

void COMMAND_pwmStartCallback(char* token)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void COMMAND_pwmStopCallback(char* token)
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

void COMMAND_pwmDcCallback(char* token)
{
	token = strtok(NULL, " ");
	int32_t dc = atoi(token);
	if (dc > 100) dc = 100;
	if (dc < 0)   dc = 0;
	htim3.Instance->CCR1 = dc;
}

void COMMAND_crcWholeFlashCalcCallback(char* token)
{
	char temp [9];
	uint32_t crcFlashResult;
	uint32_t flashSize = 0x20000;
	uint32_t *p_flash_start_address = (uint32_t *) FLASH_START_ADDRESS;

	crcFlashResult = HAL_CRC_Calculate(&hcrc, p_flash_start_address, flashSize);

	sprintf(temp, "%x\n", (unsigned int)crcFlashResult);
	COM_uartPrint(temp);
}

void COMMAND_iwdgTestCallback(char* token)
{
#ifdef IWDG_ENABLE
	while(1);
#endif
}

void COMMAND_flashLockCallback(char* token)
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
		FLASH_setSectorAddress(7, sector_7_addr);
		uint32_t myTestWrite[1] = {0xDEADBEEF}; 		//0xFFFFFFFF, 0xDEADBEEF
		FLASH_writeN(131072-4, myTestWrite2, 1, DATA_TYPE_32);
		*/
	}
}

void COMMAND_setSNCallback(char* token)
{
	//if (FLASH_wrpSectorDisable();

	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	FLASH_setSectorAddress(7, sector_7_addr);
	uint32_t myTestWrite[1] = {0xDEADBEEF}; //0xFFFFFFFF, 0xDEADBEEF
	FLASH_writeN(0, myTestWrite, 1, DATA_TYPE_32);

	//FLASH_wrpSectorEnable();

}

void COMMAND_getSNCallback(char* token)
{
	char temp [9];
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	FLASH_setSectorAddress(7, sector_7_addr);
	uint32_t myTestRead[1];
	FLASH_readN(0, myTestRead, 1, DATA_TYPE_32);

	sprintf(temp, "%x\n", (unsigned int)myTestRead[0]);
	COM_uartPrint(temp);
}

void COMMAND_startTickCallback(char* token)
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

void COMMAND_stopTickCallback(char* token)
{
	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
}

void COMMAND_assert0Callback(char* token)
{
	assert_param(0);
}

void COMMAND_clearAssertFlagCallback(char* token)
{
	SYSTEM_DEBUG_assertResetFlag();
}

void COMMAND_lsm6dslPerSampleEnableCallback(char* token)
{
	LSM6DSL_perSampleInit();
}

void COMMAND_lsm6dslFifoEnableCallback(char* token)
{
	LSM6DSL_fifoInit();
}

void COMMAND_lsm6dslDisableCallback(char* token)
{
	LSM6DSL_modesDisable();
}

void COMMAND_enterStopModeCallback(char* token)
{
	token = strtok(NULL, " ");
	uint32_t wake_up_time = atoi(token);
	SYSTEM_DEBUG_enterStopMode(wake_up_time);
}

void COMMAND_enterStandbyModeCallback(char* token)
{
	SYSTEM_DEBUG_enterStandbyMode();
}

