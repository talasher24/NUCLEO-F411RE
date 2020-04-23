/*
 * Commands.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

#ifndef COMMANDS_H
#define COMMANDS_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define NUM_OF_COMMANDS 19

#define PING_COMMAND_NAME "ping\n"

#define GET_VERSION_COMMAND_NAME "get_version\n"

#define PWM_START_COMMAND_NAME "pwm_start\n"

#define PWM_STOP_COMMAND_NAME "pwm_stop\n"

#define PWM_DC_COMMAND_NAME "pwm_dc"

#define CRC_WHOLE_FLASH_CALC "crc_whole_flash_calc\n"

#define IWDG_TEST "iwdg_test\n"

#define FLASH_LOCK "flash_lock\n"

#define set_SN "set_SN\n"

#define get_SN "get_SN\n"

#define START_TICK "start_tick\n"

#define STOP_TICK "stop_tick\n"

#define ASSERT_0 "assert_0\n"

#define CLEAR_ASSERT_FLAG "clear_assert_flag\n"

#define LSM6DSL_PER_SAMPLE_ENABLE "lsm6dsl_per_sample_enable\n"

#define LSM6DSL_FIFO_ENABLE "lsm6dsl_fifo_enable\n"

#define LSM6DSL_DISABLE "lsm6dsl_disable\n"

#define ENTER_STOP_MODE "enter_stop_mode\n"

#define ENTER_STANDBY_MODE "enter_standby_mode\n"
/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */

 typedef struct {
 	char* _name;
 	uint8_t _size;
 	void(*func_ptr)(char*);
 }command;

 void pingCallback(char* token);
 void getVersionCallback(char* token);
 void pwmStartCallback(char* token);
 void pwmStopCallback(char* token);
 void pwmDcCallback(char* token);
 void crcWholeFlashCalcCallback(char* token);
 void iwdgTestCallback(char* token);
 void flashLockCallback(char* token);
 void setSNCallback(char* token);
 void getSNCallback(char* token);
 void startTickCallback(char* token);
 void stopTickCallback(char* token);
 void assert0Callback(char* token);
 void clearAssertFlagCallback(char* token);
 void lsm6dslPerSampleEnableCallback(char* token);
 void lsm6dslFifoEnableCallback(char* token);
 void lsm6dslDisableCallback(char* token);
 void enterStopModeCallback(char* token);
 void enterStandbyModeCallback(char* token);

 static const command commands [NUM_OF_COMMANDS] = {
 		{PING_COMMAND_NAME, 			sizeof(PING_COMMAND_NAME), 			pingCallback						},
 		{GET_VERSION_COMMAND_NAME, 		sizeof(GET_VERSION_COMMAND_NAME), 	getVersionCallback					},
 		{PWM_START_COMMAND_NAME, 		sizeof(PWM_START_COMMAND_NAME), 	pwmStartCallback					},
 		{PWM_STOP_COMMAND_NAME, 		sizeof(PWM_STOP_COMMAND_NAME), 		pwmStopCallback						},
 		{PWM_DC_COMMAND_NAME, 			sizeof(PWM_DC_COMMAND_NAME), 		pwmDcCallback						},
 		{CRC_WHOLE_FLASH_CALC, 			sizeof(CRC_WHOLE_FLASH_CALC), 		crcWholeFlashCalcCallback			},
 		{IWDG_TEST, 					sizeof(IWDG_TEST), 					iwdgTestCallback					},
 		{FLASH_LOCK, 					sizeof(FLASH_LOCK), 				flashLockCallback					},
 		{set_SN, 						sizeof(set_SN), 					setSNCallback						},
 		{get_SN, 						sizeof(get_SN), 					getSNCallback						},
 		{START_TICK, 					sizeof(START_TICK), 				startTickCallback					},
 		{STOP_TICK, 					sizeof(STOP_TICK), 					stopTickCallback					},
 		{ASSERT_0, 						sizeof(ASSERT_0), 					assert0Callback						},
 		{CLEAR_ASSERT_FLAG, 			sizeof(CLEAR_ASSERT_FLAG), 			clearAssertFlagCallback				},
 		{LSM6DSL_PER_SAMPLE_ENABLE, 	sizeof(LSM6DSL_PER_SAMPLE_ENABLE), 	lsm6dslPerSampleEnableCallback		},
 		{LSM6DSL_FIFO_ENABLE, 			sizeof(LSM6DSL_FIFO_ENABLE), 		lsm6dslFifoEnableCallback			},
 		{LSM6DSL_DISABLE, 				sizeof(LSM6DSL_DISABLE), 			lsm6dslDisableCallback				},
 		{ENTER_STOP_MODE, 				sizeof(ENTER_STOP_MODE), 			enterStopModeCallback				},
 		{ENTER_STANDBY_MODE, 			sizeof(ENTER_STANDBY_MODE), 		enterStandbyModeCallback			}
 };

/* USER CODE END Prototypes */
#ifdef __cplusplus
}
#endif
#endif /* COMMANDS_H */
