/*
 * Commands.h
 *
 *  Created on: Mar 26, 2020
 *      Author: ADMIN
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

void whichCommand(void);
void ping_callBack(char* token);
void version_callback(char* token);
void pwm_start_callback(char* token);
void pwm_stop_callback(char* token);
void pwm_dc_callback(char* token);

/*------------Commands------------*/

#define NUM_OF_COMMANDS 5

#define PING_COMMAND_NAME "ping"

#define VERSION_COMMAND_NAME "version"

#define PWM_START_COMMAND_NAME "pwm_start"

#define PWM_STOP_COMMAND_NAME "pwm_stop"

#define PWM_DC_COMMAND_NAME "pwm_dc"

/*--------End_of_Commands--------*/

typedef struct {
	char* _name;
	uint8_t _size;
	void(*func_ptr)(char*);
}command;

command commands [NUM_OF_COMMANDS] = {
		{PING_COMMAND_NAME, sizeof(PING_COMMAND_NAME), ping_callBack},
		{VERSION_COMMAND_NAME, sizeof(m_p_version), version_callback},
		{PWM_START_COMMAND_NAME, sizeof(PWM_START_COMMAND_NAME), pwm_start_callback},
		{PWM_STOP_COMMAND_NAME, sizeof(PWM_STOP_COMMAND_NAME), pwm_stop_callback},
		{PWM_DC_COMMAND_NAME, sizeof(PWM_DC_COMMAND_NAME), pwm_dc_callback}
};




#endif /* COMMANDS_H_ */
