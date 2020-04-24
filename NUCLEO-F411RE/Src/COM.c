/*
 * com.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

/******************************************************************************
* Includes
*******************************************************************************/

#include <string.h>
#include "main.h"
#include "usart.h"
#include "com.h"
#include "command.h"
#include "types.h"

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
	uint8_t p_rx_buffer[BUFFER_SIZE];
	uint8_t rx_index;
	uint8_t rx_single_char;
	bool 	rx_ready_command;
	uint8_t p_tx_buffer[BUFFER_SIZE];
	bool 	tx_busy;
} uart_buffer_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

static uart_buffer_t Uart_Buffer;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

void COM_setReadyCommandFlagOn(void);
void COM_bufferInit(uint8_t* p_buffer);
void COM_setTxBusyFlagOn(void);

/******************************************************************************
* Function Definitions
*******************************************************************************/

bool COM_getReadyCommandFlag(void)
{
	return Uart_Buffer.rx_ready_command;
}

void COM_setReadyCommandFlagOn(void)
{
	Uart_Buffer.rx_ready_command = true;
}

void COM_setReadyCommandFlagOff(void)
{
	Uart_Buffer.rx_ready_command = false;
}

void COM_readyCommandProcess(void)
{
	char* token = strtok((char*)Uart_Buffer.p_rx_buffer, " ");
	COMMAND_findCommand(token);
	COM_bufferInit(Uart_Buffer.p_rx_buffer);
	Uart_Buffer.rx_index = 0;
}

void COM_uartPrint(char* token)
{
	while (COM_getTxBusyFlag());

	strncpy((char*)Uart_Buffer.p_tx_buffer, token, sizeof(Uart_Buffer.p_tx_buffer));
	while (HAL_UART_Transmit_DMA(&huart2, Uart_Buffer.p_tx_buffer, strlen(token)) != HAL_OK);
	COM_setTxBusyFlagOn();
}

void COM_halUartReceiveDma(void)
{
	HAL_UART_Receive_DMA(&huart2, &Uart_Buffer.rx_single_char, 1);
}

void COM_setTxBusyFlagOn(void)
{
	Uart_Buffer.tx_busy = true;
}

void COM_setTxBusyFlagOff(void)
{
	Uart_Buffer.tx_busy = false;
}

bool COM_getTxBusyFlag(void)
{
	return Uart_Buffer.tx_busy;
}

void COM_charHandler(void)
{
	if (COM_getReadyCommandFlag())
	{
		return;
	}

	if (Uart_Buffer.rx_single_char != '\n')
	{
		if(Uart_Buffer.rx_index < BUFFER_SIZE)
	{
			Uart_Buffer.p_rx_buffer[Uart_Buffer.rx_index] = Uart_Buffer.rx_single_char;
			Uart_Buffer.rx_index++;
	}
	}
	else
	{
		Uart_Buffer.p_rx_buffer[Uart_Buffer.rx_index] = Uart_Buffer.rx_single_char;
		Uart_Buffer.rx_index++;
		COM_setReadyCommandFlagOn();
	}
}

void COM_bufferInit(uint8_t* p_buffer)
{
	for(int i = 0; i < BUFFER_SIZE; i++){
		p_buffer[i] = 0;
	}
}


