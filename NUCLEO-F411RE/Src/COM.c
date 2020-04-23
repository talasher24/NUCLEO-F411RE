/*
 * Buffer.c
 *
 *  Created on: Apr 6, 2020
 *      Author: ADMIN
 */
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "usart.h"

#include "Types.h"
#include "COM.h"
#include "Commands.h"

static s_Buff s_uart_buffer;

void readyCommandProcess(void)
{
	if (s_uart_buffer._rx_ready_command)
	{
		COM_whichCommand();
		bufferInit(s_uart_buffer._p_rx_buffer);
		s_uart_buffer._rx_index = 0;
		s_uart_buffer._rx_ready_command = false;
	}
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
	txBusyFlagEnable();
}

void halUartReceiveDma(void)
{
	HAL_UART_Receive_DMA(&huart2, &s_uart_buffer._rx_single_char, 1);
}

void txBusyFlagEnable(void)
{
	s_uart_buffer.tx_busy = true;
}

void txBusyFlagDisable(void)
{
	s_uart_buffer.tx_busy = false;
}

bool getTxBusyFlag(void)
{
	return s_uart_buffer.tx_busy;
}

void charHandler(void)
{
	if (s_uart_buffer._rx_ready_command)
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
		s_uart_buffer._rx_ready_command = true;
	}
}

void bufferInit(uint8_t* rxBuffer)
{
	for(int i = 0; i < BUFFER_SIZE; i++){
		rxBuffer[i] = 0;
	}
}


