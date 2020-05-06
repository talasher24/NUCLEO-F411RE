/*
 * com.c
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
#include <string.h>

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

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
extern osMailQId  txMailQueueHandle;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

static void COM_setReadyCommandFlagOn(void);
static void COM_setReadyCommandFlagOff(void);
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

static void COM_setReadyCommandFlagOff(void)
{
	Uart_Buffer.rx_ready_command = false;
}

void COM_readyCommandProcess(void)
{
	char* token = strtok((char*)Uart_Buffer.p_rx_buffer, " ");
	COMMAND_findAndExecuteCommand(token);
	COM_bufferInit(Uart_Buffer.p_rx_buffer);
	COM_setReadyCommandFlagOff();
}

void COM_uartPrint(char* token)
{
	queue_message_t *queue_msg_set;
	queue_msg_set = osMailAlloc(txMailQueueHandle, 0);
	strncpy((char*)queue_msg_set->p_buffer, token, sizeof(queue_msg_set->p_buffer));
	osMailPut(txMailQueueHandle, queue_msg_set);
	if (!COM_getTxBusyFlag())
	{
		HAL_UART_TxCpltCallback(&huart2);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
	UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
	osEvent evt;
	evt = osMailGet(txMailQueueHandle, 0);
	if (evt.status != osEventMail)
	{
		COM_setTxBusyFlagOff();
	}
	else
	{
		queue_message_t *queue_msg_get;
		queue_msg_get = evt.value.p;
		strncpy((char*)Uart_Buffer.p_tx_buffer, (char*)queue_msg_get->p_buffer, sizeof(Uart_Buffer.p_tx_buffer));
		osMailFree(txMailQueueHandle, queue_msg_get);
		HAL_UART_Transmit_DMA(&huart2, Uart_Buffer.p_tx_buffer, strlen((char*)Uart_Buffer.p_tx_buffer));
		COM_setTxBusyFlagOn();
	}
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

bool COM_charHandler(void)
{
	if (COM_getReadyCommandFlag())
	{
		return false;
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
		return true;
	}
	return false;
}

static void COM_bufferInit(uint8_t* p_buffer)
{
	memset(Uart_Buffer.p_rx_buffer, 0, sizeof(uint8_t) * Uart_Buffer.rx_index);
	Uart_Buffer.rx_index = 0;
}
