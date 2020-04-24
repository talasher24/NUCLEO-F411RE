/*
 * system_isr.c
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

 /******************************************************************************
 * Includes
 *******************************************************************************/

#include "main.h"
#include "system_isr.h"
#include "com.h"
#include "lsm6dsl.h"
#include "types.h"

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

 /******************************************************************************
 * Function Prototypes
 *******************************************************************************/

 /******************************************************************************
 * Function Definitions
 *******************************************************************************/

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
  COM_setTxBusyFlagOff();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
  COM_halUartReceiveDma();

  COM_charHandler();
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
	  LSM6DSL_setInterruptFlagOn();
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
