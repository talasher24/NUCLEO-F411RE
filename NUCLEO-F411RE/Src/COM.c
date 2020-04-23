/*
 * Buffer.c
 *
 *  Created on: Apr 6, 2020
 *      Author: Tal Asher
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "main.h"
#include "COM.h"

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
	uint8_t _p_rx_buffer[BUFFER_SIZE];
	uint8_t _rx_index;
	uint8_t _rx_single_char;
	bool _rx_ready_command;
	uint8_t _p_tx_buffer[BUFFER_SIZE];
	bool tx_busy;
}s_Buff;
/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
static s_Buff s_uart_buffer;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
void setReadyCommandFlagOn(void);
void setReadyCommandFlagOff(void);
void COM_whichCommand(void);
void bufferInit(uint8_t* rxBuffer);
void setTxBusyFlagOn(void);
/******************************************************************************
* Function Definitions
*******************************************************************************/

bool getReadyCommandFlag(void)
{
	return s_uart_buffer._rx_ready_command;
}

void setReadyCommandFlagOn(void)
{
	s_uart_buffer._rx_ready_command = true;
}

void setReadyCommandFlagOff(void)
{
	s_uart_buffer._rx_ready_command = false;
}

void readyCommandProcess(void)
{
	COM_whichCommand();
	bufferInit(s_uart_buffer._p_rx_buffer);
	s_uart_buffer._rx_index = 0;
	setReadyCommandFlagOff();
}

void COM_whichCommand (void)
{
	char* token = strtok((char*)s_uart_buffer._p_rx_buffer, " ");

	for (uint8_t i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if (strncmp(token, commands[i]._name, commands[i]._size)==0)
		{
			uartPrint(OK);
			commands[i].func_ptr(token);
			return;
		}
	}
}

void uartPrint(char* token)
{
	while (getTxBusyFlag());

	//memcpy((char*)s_uart_buffer._p_tx_buffer, token, sizeof(s_uart_buffer._p_tx_buffer));
	strncpy((char*)s_uart_buffer._p_tx_buffer, token, sizeof(s_uart_buffer._p_tx_buffer));

	while (HAL_UART_Transmit_DMA(&huart2, s_uart_buffer._p_tx_buffer, strlen(token)) != HAL_OK);
	setTxBusyFlagOn();
}

void halUartReceiveDma(void)
{
	HAL_UART_Receive_DMA(&huart2, &s_uart_buffer._rx_single_char, 1);
}

void setTxBusyFlagOn(void)
{
	s_uart_buffer.tx_busy = true;
}

void setTxBusyFlagOff(void)
{
	s_uart_buffer.tx_busy = false;
}

bool getTxBusyFlag(void)
{
	return s_uart_buffer.tx_busy;
}

void charHandler(void)
{
	if (getReadyCommandFlag())
	{
		return;
	}

	if (s_uart_buffer._rx_single_char != '\n')
	{
		if(s_uart_buffer._rx_index < BUFFER_SIZE)
	{
			s_uart_buffer._p_rx_buffer[s_uart_buffer._rx_index] = s_uart_buffer._rx_single_char;
			s_uart_buffer._rx_index++;
	}
	}
	else
	{
		s_uart_buffer._p_rx_buffer[s_uart_buffer._rx_index] = s_uart_buffer._rx_single_char;
		s_uart_buffer._rx_index++;
		setReadyCommandFlagOn();
	}
}

void bufferInit(uint8_t* rxBuffer)
{
	for(int i = 0; i < BUFFER_SIZE; i++){
		rxBuffer[i] = 0;
	}
}


