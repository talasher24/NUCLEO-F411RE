/*
 * system_debug.c
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

/******************************************************************************
* Includes
*******************************************************************************/

#include "stm32f4xx_hal.h"
#include "system_debug.h"
#include "com.h"
#include "rtc.h"
#include <string.h>
#include <stdio.h>

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

#define ASSERT_FLAG_ON 0xAA
#define ASSERT_FLAG_OFF 0x55

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

typedef enum reset_cause_t {
    RESET_CAUSE_UNKNOWN = 0,
    RESET_CAUSE_LOW_POWER_RESET,
    RESET_CAUSE_WINDOW_WATCHDOG_RESET,
    RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
    RESET_CAUSE_SOFTWARE_RESET,
    RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
    RESET_CAUSE_EXTERNAL_RESET_PIN_RESET,
    RESET_CAUSE_BROWNOUT_RESET,
} reset_cause_t;

typedef struct {
	char 		p_file [60];
	uint32_t 	line;
	uint8_t 	flag;
} assert_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

static reset_cause_t Reset_Cause;
__attribute__((section(".noinit"))) static assert_t Assert_Struct;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

extern void SystemClock_Config(void);
reset_cause_t SYSTEM_DEBUG_resetCauseGet(void);
const char * SYSTEM_DEBUG_resetCauseGetName(reset_cause_t Reset_Cause);

/******************************************************************************
* Function Definitions
*******************************************************************************/

void SYSTEM_DEBUG_assertRecord(uint8_t *p_file, uint32_t line)
{
	Assert_Struct.flag = ASSERT_FLAG_ON;
	for (int i = 0; i < sizeof(Assert_Struct.p_file); i++)
	{
		Assert_Struct.p_file [i]  = 0;
	}

	strncpy((char*)Assert_Struct.p_file, (char*)p_file, sizeof(Assert_Struct.p_file));
	Assert_Struct.line = line;

	while (COM_getTxBusyFlag()); // OK message
	NVIC_SystemReset();
	while (1) {}; // never reached to here
}

void SYSTEM_DEBUG_assertMsgPrint(void)
{
	if (Assert_Struct.flag == ASSERT_FLAG_ON)
	{
		char temp [100];
		sprintf(temp, "Problem found! Path: %s\tLine: %u\n", Assert_Struct.p_file, (unsigned int)Assert_Struct.line);
		COM_uartPrint(temp);
	}
}

void SYSTEM_DEBUG_assertResetFlag(void)
{
	Assert_Struct.flag = ASSERT_FLAG_OFF;
}

void SYSTEM_DEBUG_printResetCause(void)
{
	Reset_Cause = SYSTEM_DEBUG_resetCauseGet();

	char temp [100];
	sprintf(temp, "The system reset cause is \%s\"\n", SYSTEM_DEBUG_resetCauseGetName(Reset_Cause));
	COM_uartPrint(temp);
}

reset_cause_t SYSTEM_DEBUG_resetCauseGet(void)
{
    reset_cause_t Reset_Cause;

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
    {
        Reset_Cause = RESET_CAUSE_LOW_POWER_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
    {
        Reset_Cause = RESET_CAUSE_WINDOW_WATCHDOG_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
    {
        Reset_Cause = RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
    {
        Reset_Cause = RESET_CAUSE_SOFTWARE_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
    {
        Reset_Cause = RESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
    {
        Reset_Cause = RESET_CAUSE_EXTERNAL_RESET_PIN_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
    {
        Reset_Cause = RESET_CAUSE_BROWNOUT_RESET;
    }
    else
    {
        Reset_Cause = RESET_CAUSE_UNKNOWN;
    }

    __HAL_RCC_CLEAR_RESET_FLAGS();

    return Reset_Cause;
}

const char * SYSTEM_DEBUG_resetCauseGetName(reset_cause_t Reset_Cause)
{
    const char * reset_cause_name = "TBD";

    switch (Reset_Cause)
    {
        case RESET_CAUSE_UNKNOWN:
            reset_cause_name = "UNKNOWN";
            break;
        case RESET_CAUSE_LOW_POWER_RESET:
            reset_cause_name = "LOW_POWER_RESET";
            break;
        case RESET_CAUSE_WINDOW_WATCHDOG_RESET:
            reset_cause_name = "WINDOW_WATCHDOG_RESET";
            break;
        case RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET:
            reset_cause_name = "INDEPENDENT_WATCHDOG_RESET";
            break;
        case RESET_CAUSE_SOFTWARE_RESET:
            reset_cause_name = "SOFTWARE_RESET";
            break;
        case RESET_CAUSE_POWER_ON_POWER_DOWN_RESET:
            reset_cause_name = "POWER-ON_RESET (POR) / POWER-DOWN_RESET (PDR)";
            break;
        case RESET_CAUSE_EXTERNAL_RESET_PIN_RESET:
            reset_cause_name = "EXTERNAL_RESET_PIN_RESET";
            break;
        case RESET_CAUSE_BROWNOUT_RESET:
            reset_cause_name = "BROWNOUT_RESET (BOR)";
            break;
    }

    return reset_cause_name;
}

void SYSTEM_DEBUG_enterSleepMode(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

	// Enters to sleep mode
	HAL_SuspendTick();
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	HAL_ResumeTick();

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}

void SYSTEM_DEBUG_enterStopMode(uint32_t wake_up_time)
{
	const float wakeup_time_base = 0.00048;
	uint32_t wakeup_counter = wake_up_time / wakeup_time_base;

	COM_uartPrint("STOP MODE is ON\n");

	/* enable the RTC Wakeup */
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, wakeup_counter, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	{
		Error_Handler();
	}

	// Enters to stop mode
	while (COM_getTxBusyFlag());
	HAL_SuspendTick();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

	SystemClock_Config();
	HAL_ResumeTick();

	/** Deactivate the RTC wakeup  **/
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

	COM_uartPrint("STOP MODE is OFF\n");
}

void SYSTEM_DEBUG_enterStandbyMode(void)
{
	/* Clear the WU FLAG */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	 /* clear the RTC Wake UP (WU) flag */
	 __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

	 /* Enable the WAKEUP PIN */
	 HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	 /* enable the RTC Wakeup */
	    /*  RTC Wake-up Interrupt Generation:
	      Wake-up Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI))
	      ==> WakeUpCounter = Wake-up Time / Wake-up Time Base

	      To configure the wake up timer to 5s the WakeUpCounter is set to 0x28b0:
	      RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
	      Wake-up Time Base = 16 /(32.768KHz) = 0.00048 seconds
	      ==> WakeUpCounter = ~5s/0.00048s = 10416 = 0x28b0
	    */
	 if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x2710, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	 {
		 Error_Handler();
	 }
	 COM_uartPrint("STANDBY MODE is ON\n");

	 /* Enter the standby mode */
	 HAL_PWR_EnterSTANDBYMode();
}

void SYSTEM_DEBUG_wakeupStandbyMode(void)
{
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);  // clear the flag

		COM_uartPrint("Wakeup from STANDBY MODE\n");

		/** Disable the WWAKEUP PIN **/
		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);  // disable PA0

		/** Deactivate the RTC wakeup  **/
		HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	}
}


