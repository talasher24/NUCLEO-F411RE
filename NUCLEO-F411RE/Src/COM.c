/*
 * com.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

/******************************************************************************
* Includes
*******************************************************************************/

#include "com.h"
#include "usart.h"
#include "command.h"
#include "types.h"
#include "stm32f4xx_hal.h"
#include <string.h>

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

static void COM_setReadyCommandFlagOn(void);
static void COM_bufferInit(uint8_t* p_buffer);
static void COM_setTxBusyFlagOn(void);

/******************************************************************************
* Function Definitions
*******************************************************************************/

bool COM_getReadyCommandFlag(void)
{
	return Uart_Buffer.rx_ready_command;
}

static void COM_setReadyCommandFlagOn(void)
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
	COMMAND_findAndExecuteCommand(token);
	COM_bufferInit(Uart_Buffer.p_rx_buffer);
}

void COM_uartPrint(char* token)
{
	while (COM_getTxBusyFlag());
	strncpy((char*)Uart_Buffer.p_tx_buffer, token, sizeof(Uart_Buffer.p_tx_buffer));
	HAL_UART_Transmit_DMA(&huart2, Uart_Buffer.p_tx_buffer, strlen(token));
	COM_setTxBusyFlagOn();
}

void COM_halUartReceiveDma(void)
{
	HAL_UART_Receive_DMA(&huart2, &Uart_Buffer.rx_single_char, 1);
}

static void COM_setTxBusyFlagOn(void)
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
		COM_setReadyCommandFlagOn();
	}
}

static void COM_bufferInit(uint8_t* p_buffer)
{
	memset(Uart_Buffer.p_rx_buffer, 0, sizeof(uint8_t) * Uart_Buffer.rx_index);
	Uart_Buffer.rx_index = 0;
}


