/*
 * system_isr.c
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

 /******************************************************************************
 * Includes
 *******************************************************************************/

#include "stm32f4xx_hal.h"
#include "system_isr.h"
#include "com.h"
#include "lsm6dsl.h"
#include "types.h"
#include "cmsis_os.h"

 /******************************************************************************
 * Module Preprocessor Constants
 *******************************************************************************/

 /******************************************************************************
 * Module Preprocessor Macros
 *******************************************************************************/

 /******************************************************************************
 * Module Typedefs
 *******************************************************************************/

 /******************************************************************************
 * Module Variable Definitions
 *******************************************************************************/

extern osThreadId lsm6dslTaskHandle;
osTimerId Timer01Handle;

 /******************************************************************************
 * Function Prototypes
 *******************************************************************************/

void Timer01Callback(void const * argument);

 /******************************************************************************
 * Function Definitions
 *******************************************************************************/

void SYSTEM_ISR_init(void)
{
	osTimerDef(Timer01, Timer01Callback);
	Timer01Handle = osTimerCreate(osTimer(Timer01), osTimerPeriodic, NULL);
}

void SYSTEM_ISR_osTimer01Start(uint32_t timer_period_milisec)
{
	osTimerStart(Timer01Handle, timer_period_milisec);
}

void SYSTEM_ISR_osTimer01Stop(void)
{
	osTimerStop(Timer01Handle);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(GPIO_Pin);
	/* NOTE: This function Should not be modified, when the callback is needed,
		   the HAL_GPIO_EXTI_Callback could be implemented in the user file
	*/
	if (GPIO_Pin == GPIO_PIN_5)
	{
		osSignalSet(lsm6dslTaskHandle, LSM6DSL_SIGNAL);
	}
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
	UNUSED(hrtc);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTC_AlarmAEventCallback could be implemented in the user file
   */
	COM_uartPrint(TICK);
}

void Timer01Callback(void const * argument)
{
  /* USER CODE BEGIN Timer01Callback */
	COM_uartPrint(TIMER01_MESSAGE);
  /* USER CODE END Timer01Callback */
}
