/*
 * Com.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

#ifndef COM_H
#define COM_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */

 bool getReadyCommandFlag(void);
 void readyCommandProcess(void);
 void uartPrint(char* token);
 void halUartReceiveDma(void);
 void charHandler(void);
 void setTxBusyFlagOff(void);
 bool getTxBusyFlag(void);

/* USER CODE END Prototypes */
#ifdef __cplusplus
}
#endif
#endif // COM_H


