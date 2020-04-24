/*
 * command.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

#ifndef COMMAND_H
#define COMMAND_H
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

 /******************************************************************************
 * Function Prototypes
 *******************************************************************************/

 void COMMAND_findCommand(char* token);

 void COMMAND_pingCallback(char* token);
 void COMMAND_getVersionCallback(char* token);
 void COMMAND_pwmStartCallback(char* token);
 void COMMAND_pwmStopCallback(char* token);
 void COMMAND_pwmDcCallback(char* token);
 void COMMAND_crcWholeFlashCalcCallback(char* token);
 void COMMAND_iwdgTestCallback(char* token);
 void COMMAND_flashLockCallback(char* token);
 void COMMAND_setSNCallback(char* token);
 void COMMAND_getSNCallback(char* token);
 void COMMAND_startTickCallback(char* token);
 void COMMAND_stopTickCallback(char* token);
 void COMMAND_assert0Callback(char* token);
 void COMMAND_clearAssertFlagCallback(char* token);
 void COMMAND_lsm6dslPerSampleEnableCallback(char* token);
 void COMMAND_lsm6dslFifoEnableCallback(char* token);
 void COMMAND_lsm6dslDisableCallback(char* token);
 void COMMAND_enterStopModeCallback(char* token);
 void COMMAND_enterStandbyModeCallback(char* token);

 /******************************************************************************
 * Function Definitions
 *******************************************************************************/


#ifdef __cplusplus
}
#endif
#endif /* COMMANDS_H */
