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
#include "system_isr.h"

#include "rtc.h"
#include "tim.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

#define NUM_OF_COMMANDS 21

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

#define START_OS_TIMER "start_os_timer"

#define STOP_OS_TIMER "stop_os_timer"

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

 typedef struct {
	char* 	p_name;
	uint8_t size;
	void 	(*func_ptr)(char*);
 } command_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

 const command_t P_COMMANDS [NUM_OF_COMMANDS] = {
		{PING_COMMAND_NAME, 			strlen(PING_COMMAND_NAME), 			COMMAND_pingCallback						},
		{GET_VERSION_COMMAND_NAME, 		strlen(GET_VERSION_COMMAND_NAME), 	COMMAND_getVersionCallback					},
		{PWM_START_COMMAND_NAME, 		strlen(PWM_START_COMMAND_NAME), 	COMMAND_pwmStartCallback					},
		{PWM_STOP_COMMAND_NAME, 		strlen(PWM_STOP_COMMAND_NAME), 		COMMAND_pwmStopCallback						},
		{PWM_DC_COMMAND_NAME, 			strlen(PWM_DC_COMMAND_NAME), 		COMMAND_pwmDcCallback						},
		{CRC_WHOLE_FLASH_CALC, 			strlen(CRC_WHOLE_FLASH_CALC), 		COMMAND_crcWholeFlashCalcCallback			},
		{IWDG_TEST, 					strlen(IWDG_TEST), 					COMMAND_iwdgTestCallback					},
		{FLASH_LOCK, 					strlen(FLASH_LOCK), 				COMMAND_flashLockCallback					},
		{set_SN, 						strlen(set_SN), 					COMMAND_setSNCallback						},
		{get_SN, 						strlen(get_SN), 					COMMAND_getSNCallback						},
		{START_TICK, 					strlen(START_TICK), 				COMMAND_startTickCallback					},
		{STOP_TICK, 					strlen(STOP_TICK), 					COMMAND_stopTickCallback					},
		{ASSERT_0, 						strlen(ASSERT_0), 					COMMAND_assert0Callback						},
		{CLEAR_ASSERT_FLAG, 			strlen(CLEAR_ASSERT_FLAG), 			COMMAND_clearAssertFlagCallback				},
		{LSM6DSL_PER_SAMPLE_ENABLE, 	strlen(LSM6DSL_PER_SAMPLE_ENABLE), 	COMMAND_lsm6dslPerSampleEnableCallback		},
		{LSM6DSL_FIFO_ENABLE, 			strlen(LSM6DSL_FIFO_ENABLE), 		COMMAND_lsm6dslFifoEnableCallback			},
		{LSM6DSL_DISABLE, 				strlen(LSM6DSL_DISABLE), 			COMMAND_lsm6dslDisableCallback				},
		{ENTER_STOP_MODE, 				strlen(ENTER_STOP_MODE), 			COMMAND_enterStopModeCallback				},
		{ENTER_STANDBY_MODE, 			strlen(ENTER_STANDBY_MODE), 		COMMAND_enterStandbyModeCallback			},
		{START_OS_TIMER, 				strlen(START_OS_TIMER), 			COMMAND_startOsTimerCallback				},
		{STOP_OS_TIMER, 				strlen(STOP_OS_TIMER), 				COMMAND_stopOsTimerCallback					}
 };

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

 /**
   * @brief 	XXX
   * @param 	XXX
   * @retval	XXX
   */
void COMMAND_findAndExecuteCommand(char* p_token)
{
	for (uint8_t i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if (strncmp(p_token, P_COMMANDS[i].p_name, P_COMMANDS[i].size)==0)
		{
			COM_uartPrint(OK);
			P_COMMANDS[i].func_ptr(p_token);
			return;
		}
	}
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_pingCallback(char* p_token)
{
	COM_uartPrint(PING);
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_getVersionCallback(char* p_token)
{
	COM_uartPrint(VERSION);
}

void COMMAND_pwmStartCallback(char* p_token)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_pwmStopCallback(char* p_token)
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_pwmDcCallback(char* p_token)
{
	p_token = strtok(NULL, " ");
	int32_t dc = atoi(p_token);
	if (dc > 100) dc = 100;
	if (dc < 0)   dc = 0;
	htim3.Instance->CCR1 = dc;
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_crcWholeFlashCalcCallback(char* p_token)
{
	FLASH_crcWholeFlashCalc();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_iwdgTestCallback(char* p_token)
{
#ifdef IWDG_ENABLE
	while(1);
#endif
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_flashLockCallback(char* p_token)
{
	FLASH_lock();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_setSNCallback(char* p_token)
{
	FLASH_setSerialNumberInSector7();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_getSNCallback(char* p_token)
{
	FLASH_getSerialNumberFromSector7();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_startTickCallback(char* p_token)
{
	RTC_activateAlarmA();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_stopTickCallback(char* p_token)
{
	RTC_deactivateAlarmA();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_assert0Callback(char* p_token)
{
	assert_param(0);
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_clearAssertFlagCallback(char* p_token)
{
	SYSTEM_DEBUG_assertResetFlag();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_lsm6dslPerSampleEnableCallback(char* p_token)
{
	LSM6DSL_perSampleInit();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_lsm6dslFifoEnableCallback(char* p_token)
{
	LSM6DSL_fifoInit();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_lsm6dslDisableCallback(char* p_token)
{
	LSM6DSL_modesDisable();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_enterStopModeCallback(char* p_token)
{
	p_token = strtok(NULL, " ");
	uint32_t wake_up_time = atoi(p_token);
	SYSTEM_DEBUG_enterStopMode(wake_up_time);
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_enterStandbyModeCallback(char* p_token)
{
	SYSTEM_DEBUG_enterStandbyMode();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_startOsTimerCallback(char* p_token)
{
	p_token = strtok(NULL, " ");
	uint32_t timer_period_milisec = atoi(p_token) * 1000;
	SYSTEM_ISR_osTimer01Start(timer_period_milisec);
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COMMAND_stopOsTimerCallback(char* p_token)
{
	SYSTEM_ISR_osTimer01Stop();
}

