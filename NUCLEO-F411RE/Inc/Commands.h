/*
 * Commands.h
 *
 *  Created on: Mar 26, 2020
 *      Author: ADMIN
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "stm32f4xx_hal.h"

void ping_callBack(char* token);
void get_version_callback(char* token);
void pwm_start_callback(char* token);
void pwm_stop_callback(char* token);
void pwm_dc_callback(char* token);
void crc_whole_flash_calc_callback(char* token);
void iwdg_test_callback(char* token);
void flash_lock_callback(char* token);
void set_SN_callback(char* token);
void get_SN_callback(char* token);
void start_tick_callback(char* token);
void stop_tick_callback(char* token);
void assert_0_callback(char* token);
void clear_assert_flag_callback(char* token);
void lsm6dsl_per_sample_enable_callback(char* token);
void lsm6dsl_fifo_enable_callback(char* token);
void lsm6dsl_disable_callback(char* token);
void enter_stop_mode_callback(char* token);
void enter_standby_mode_callback(char* token);


/*------------Commands------------*/

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


/*--------End_of_Commands--------*/

typedef struct {
	char* _name;
	uint8_t _size;
	void(*func_ptr)(char*);
}command;


static const command commands [NUM_OF_COMMANDS] = {
		{PING_COMMAND_NAME, 			sizeof(PING_COMMAND_NAME), 			ping_callBack						},
		{GET_VERSION_COMMAND_NAME, 		sizeof(GET_VERSION_COMMAND_NAME), 	get_version_callback				},
		{PWM_START_COMMAND_NAME, 		sizeof(PWM_START_COMMAND_NAME), 	pwm_start_callback					},
		{PWM_STOP_COMMAND_NAME, 		sizeof(PWM_STOP_COMMAND_NAME), 		pwm_stop_callback					},
		{PWM_DC_COMMAND_NAME, 			sizeof(PWM_DC_COMMAND_NAME), 		pwm_dc_callback						},
		{CRC_WHOLE_FLASH_CALC, 			sizeof(CRC_WHOLE_FLASH_CALC), 		crc_whole_flash_calc_callback		},
		{IWDG_TEST, 					sizeof(IWDG_TEST), 					iwdg_test_callback					},
		{FLASH_LOCK, 					sizeof(FLASH_LOCK), 				flash_lock_callback					},
		{set_SN, 						sizeof(set_SN), 					set_SN_callback						},
		{get_SN, 						sizeof(get_SN), 					get_SN_callback						},
		{START_TICK, 					sizeof(START_TICK), 				start_tick_callback					},
		{STOP_TICK, 					sizeof(STOP_TICK), 					stop_tick_callback					},
		{ASSERT_0, 						sizeof(ASSERT_0), 					assert_0_callback					},
		{CLEAR_ASSERT_FLAG, 			sizeof(CLEAR_ASSERT_FLAG), 			clear_assert_flag_callback			},
		{LSM6DSL_PER_SAMPLE_ENABLE, 	sizeof(LSM6DSL_PER_SAMPLE_ENABLE), 	lsm6dsl_per_sample_enable_callback	},
		{LSM6DSL_FIFO_ENABLE, 			sizeof(LSM6DSL_FIFO_ENABLE), 		lsm6dsl_fifo_enable_callback		},
		{LSM6DSL_DISABLE, 				sizeof(LSM6DSL_DISABLE), 			lsm6dsl_disable_callback			},
		{ENTER_STOP_MODE, 				sizeof(ENTER_STOP_MODE), 			enter_stop_mode_callback			},
		{ENTER_STANDBY_MODE, 			sizeof(ENTER_STANDBY_MODE), 		enter_standby_mode_callback			}
};



#endif /* COMMANDS_H_ */
