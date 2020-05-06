/*
 * system_isr.c
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

 /******************************************************************************
 * Includes
 *******************************************************************************/

#include "system_isr.h"
#include "com.h"
#include "lsm6dsl.h"
#include "types.h"
#include "stm32f4xx_hal.h"
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

extern osThreadId defaultTaskHandle;

 /******************************************************************************
 * Function Prototypes
 *******************************************************************************/

 /******************************************************************************
 * Function Definitions
 *******************************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);
	/* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
	 */
	COM_halUartReceiveDma();

	if (COM_charHandler())
	{
		osSignalSet(defaultTaskHandle, READY_COMMAND_SIGNAL);
	}
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
		//LSM6DSL_setInterruptFlagOn();
		osSignalSet(defaultTaskHandle, LSM6DSL_SIGNAL);
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
