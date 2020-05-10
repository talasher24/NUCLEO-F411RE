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

 void COMMAND_findAndExecuteCommand(char* p_token);

 void COMMAND_pingCallback(char* p_token);
 void COMMAND_getVersionCallback(char* p_token);
 void COMMAND_pwmStartCallback(char* p_token);
 void COMMAND_pwmStopCallback(char* p_token);
 void COMMAND_pwmDcCallback(char* p_token);
 void COMMAND_crcWholeFlashCalcCallback(char* p_token);
 void COMMAND_iwdgTestCallback(char* p_token);
 void COMMAND_flashLockCallback(char* p_token);
 void COMMAND_setSNCallback(char* p_token);
 void COMMAND_getSNCallback(char* p_token);
 void COMMAND_startTickCallback(char* p_token);
 void COMMAND_stopTickCallback(char* p_token);
 void COMMAND_assert0Callback(char* p_token);
 void COMMAND_clearAssertFlagCallback(char* p_token);
 void COMMAND_lsm6dslPerSampleEnableCallback(char* p_token);
 void COMMAND_lsm6dslFifoEnableCallback(char* p_token);
 void COMMAND_lsm6dslDisableCallback(char* p_token);
 void COMMAND_enterStopModeCallback(char* p_token);
 void COMMAND_enterStandbyModeCallback(char* p_token);
 void COMMAND_startOsTimerCallback(char* p_token);
 void COMMAND_stopOsTimerCallback(char* p_token);

 /******************************************************************************
 * Function Definitions
 *******************************************************************************/


#ifdef __cplusplus
}
#endif
#endif /* COMMANDS_H */
