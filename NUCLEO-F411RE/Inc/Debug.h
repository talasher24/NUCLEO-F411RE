/*
 * Debug.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

#ifndef Debug_H
#define Debug_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
 //#define IWDG_ENABLE
/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */

 void assertRecord(uint8_t *file, uint32_t line);
 void assertMsgPrint(void);
 void assertResetFlag(void);

 void printResetCause(void);

 void enterSleepMode(void);
 void enterStopMode(void);
 void enterStandbyMode(void);
 void wakeupStandbyMode(void);

/* USER CODE END Prototypes */
#ifdef __cplusplus
}
#endif










#endif // Debug_H
