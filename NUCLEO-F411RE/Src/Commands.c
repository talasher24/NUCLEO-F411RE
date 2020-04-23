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
#include "Com.h"
#include "Flash.h"
#include "lsm6dsl.h"
#include "Commands.h"
#include "Types.h"
#include "Debug.h"
#include "crc.h"
#include "rtc.h"
#include "tim.h"


void pingCallback(char* token)
{
	uartPrint(token);
}

void getVersionCallback(char* token)
{
	uartPrint(VERSION);
}

void pwmStartCallback(char* token)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void pwmStopCallback(char* token)
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

void pwmDcCallback(char* token)
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

void crcWholeFlashCalcCallback(char* token)
{
	char temp [9];
	uint32_t crcFlashResult;
	uint32_t flashSize = 0x20000;
	uint32_t *p_flash_start_address = (uint32_t *) FLASH_START_ADDRESS;

	crcFlashResult = HAL_CRC_Calculate(&hcrc, p_flash_start_address, flashSize);

	sprintf(temp, "%x\n", (unsigned int)crcFlashResult);
	uartPrint(temp);
}

void iwdgTestCallback(char* token)
{
#ifdef IWDG_ENABLE
	while(1);
#endif
}

void flashLockCallback(char* token)
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
		flashSetSectorAddress(7, sector_7_addr);
		uint32_t myTestWrite[1] = {0xDEADBEEF}; 		//0xFFFFFFFF, 0xDEADBEEF
		flashWriteN(131072-4, myTestWrite2, 1, DATA_TYPE_32);
		*/
	}
}

void setSNCallback(char* token)
{
	/*if (flashWrpSectorDisable() != HAL_OK)
	{
		Error_Handler();
	}*/
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	flashSetSectorAddress(7, sector_7_addr);
	uint32_t myTestWrite[1] = {0xDEADBEEF}; //0xFFFFFFFF, 0xDEADBEEF
	flashWriteN(0, myTestWrite, 1, DATA_TYPE_32);

	/*if (flashWrpSectorEnable() != HAL_OK)
	{
		Error_Handler();
	}*/
}

void getSNCallback(char* token)
{
	char temp [9];
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	flashSetSectorAddress(7, sector_7_addr);
	uint32_t myTestRead[1];
	flashReadN(0, myTestRead, 1, DATA_TYPE_32);

	sprintf(temp, "%x\n", (unsigned int)myTestRead[0]);
	uartPrint(temp);
}

void startTickCallback(char* token)
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

void stopTickCallback(char* token)
{
	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
}

void assert0Callback(char* token)
{
	assert_param(0);
}

void clearAssertFlagCallback(char* token)
{
	assertResetFlag();
}

void lsm6dslPerSampleEnableCallback(char* token)
{
	LSM6DSL_perSampleInit();
}

void lsm6dslFifoEnableCallback(char* token)
{
	LSM6DSL_FIFO_Init();
}

void lsm6dslDisableCallback(char* token)
{
	LSM6DSL_Mode_Disable();
}

void enterStopModeCallback(char* token)
{
	enterStopMode();
}

void enterStandbyModeCallback(char* token)
{
	enterStandbyMode();
}

