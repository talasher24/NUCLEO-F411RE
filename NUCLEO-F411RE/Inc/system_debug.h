/*
 * system_debug.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

#ifndef Debug_H
#define Debug_H
#ifdef __cplusplus
 extern "C" {
#endif

 /******************************************************************************
 * Includes
 *******************************************************************************/

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

 void SYSTEM_DEBUG_assertRecord(uint8_t *p_file, uint32_t line);
 void SYSTEM_DEBUG_assertMsgPrint(void);
 void SYSTEM_DEBUG_assertResetFlag(void);

 void SYSTEM_DEBUG_printResetCause(void);

 void SYSTEM_DEBUG_enterSleepMode(void);
 void SYSTEM_DEBUG_enterStopMode(uint32_t wake_up_time);
 void SYSTEM_DEBUG_enterStandbyMode(void);
 void SYSTEM_DEBUG_wakeupStandbyMode(void);

 /******************************************************************************
 * Function Prototypes
 *******************************************************************************/

 /******************************************************************************
 * Function Definitions
 *******************************************************************************/


#ifdef __cplusplus
}
#endif
#endif // Debug_H
