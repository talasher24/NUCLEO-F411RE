/*
 * com.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

#ifndef COM_H
#define COM_H
#ifdef __cplusplus
 extern "C" {
#endif

 /******************************************************************************
 * Includes
 *******************************************************************************/

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

 /******************************************************************************
 * Module Preprocessor Constants
 *******************************************************************************/

#define BUFFER_SIZE 100

 /******************************************************************************
 * Module Preprocessor Macros
 *******************************************************************************/

 /******************************************************************************
 * Module Typedefs
 *******************************************************************************/

 typedef struct {
 	 uint8_t p_buffer[BUFFER_SIZE];
 } queue_message_t;

 /******************************************************************************
 * Module Variable Definitions
 *******************************************************************************/

 /******************************************************************************
 * Function Prototypes
 *******************************************************************************/

 bool COM_getReadyCommandFlag(void);
 void COM_readyCommandProcess(void);
 void COM_uartPrint(char* token);
 void COM_halUartReceiveDma(void);
 bool COM_charHandler(void);
 void COM_setTxBusyFlagOff(void);
 bool COM_getTxBusyFlag(void);

 /******************************************************************************
 * Function Definitions
 *******************************************************************************/


#ifdef __cplusplus
}
#endif
#endif // COM_H


