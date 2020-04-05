/*
 * Commands.h
 *
 *  Created on: Mar 26, 2020
 *      Author: ADMIN
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

void ping_callBack(char* token);
void version_callback(char* token);
void pwm_start_callback(char* token);
void pwm_stop_callback(char* token);
void pwm_dc_callback(char* token);
void crc_whole_flash_calc_callback(char* token);
void wwdg_test_callback(char* token);
void flash_lock_callback(char* token);
void set_SN_callback(char* token);
void get_SN_callback(char* token);

/*------------Commands------------*/

#define NUM_OF_COMMANDS 10

#define PING_COMMAND_NAME "ping\n"

#define VERSION_COMMAND_NAME "version\n"

#define PWM_START_COMMAND_NAME "pwm_start\n"

#define PWM_STOP_COMMAND_NAME "pwm_stop\n"

#define PWM_DC_COMMAND_NAME "pwm_dc"

#define CRC_WHOLE_FLASH_CALC "crc_whole_flash_calc\n"

#define WWDG_TEST "wwdg_test\n"

#define FLASH_LOCK "flash_lock\n"

#define set_SN "set_SN\n"

#define get_SN "get_SN\n"

/*--------End_of_Commands--------*/

typedef struct {
	char* _name;
	uint8_t _size;
	void(*func_ptr)(char*);
}command;

command commands [NUM_OF_COMMANDS] = {
		{PING_COMMAND_NAME, sizeof(PING_COMMAND_NAME), ping_callBack},
		{VERSION_COMMAND_NAME, sizeof(VERSION_COMMAND_NAME), version_callback},
		{PWM_START_COMMAND_NAME, sizeof(PWM_START_COMMAND_NAME), pwm_start_callback},
		{PWM_STOP_COMMAND_NAME, sizeof(PWM_STOP_COMMAND_NAME), pwm_stop_callback},
		{PWM_DC_COMMAND_NAME, sizeof(PWM_DC_COMMAND_NAME), pwm_dc_callback},
		{CRC_WHOLE_FLASH_CALC, sizeof(CRC_WHOLE_FLASH_CALC), crc_whole_flash_calc_callback},
		{WWDG_TEST, sizeof(WWDG_TEST), wwdg_test_callback},
		{FLASH_LOCK, sizeof(FLASH_LOCK), flash_lock_callback},
		{set_SN, sizeof(set_SN), set_SN_callback},
		{get_SN, sizeof(get_SN), get_SN_callback}
};


#endif /* COMMANDS_H_ */
