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
	uint8_t p_tx_buffer[BUFFER_SIZE];
	bool 	tx_busy;
} uart_buffer_t;

typedef struct {
	 uint8_t p_buffer[BUFFER_SIZE];
} queue_message_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

static uart_buffer_t Uart_Buffer;
static osMailQId  txMailQueueHandle;
static osMailQId  rxMailQueueHandle;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

static void COM_halUartReceiveDma(void);
static bool COM_charHandler(void);
static void COM_bufferInit(uint8_t* p_buffer);
static void COM_setTxBusyFlagOn(void);

/******************************************************************************
* Function Definitions
*******************************************************************************/

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COM_init(void)
{
	osMailQDef(txMailQueue, 16, queue_message_t);
	txMailQueueHandle = osMailCreate(osMailQ(txMailQueue), NULL);

	osMailQDef(rxMailQueue, 16, queue_message_t);
	rxMailQueueHandle = osMailCreate(osMailQ(rxMailQueue), NULL);

	COM_halUartReceiveDma();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
static void COM_halUartReceiveDma(void)
{
	HAL_UART_Receive_DMA(&huart2, &Uart_Buffer.rx_single_char, 1);
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);
	/* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
	 */
	COM_halUartReceiveDma();

	if (COM_charHandler())
	{
		queue_message_t *queue_msg_set;
		queue_msg_set = osMailAlloc(rxMailQueueHandle, 0);
		if (queue_msg_set == NULL)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			return;
		}
		strncpy((char*)queue_msg_set->p_buffer, (char*)Uart_Buffer.p_rx_buffer, sizeof(queue_msg_set->p_buffer));
		COM_bufferInit(Uart_Buffer.p_rx_buffer);
		osMailPut(rxMailQueueHandle, queue_msg_set);
	}
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
static bool COM_charHandler(void)
{
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
		return true;
	}
	return false;
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COM_readyCommandProcess(void)
{
	osEvent evt;
	evt = osMailGet(rxMailQueueHandle, 0);
	if (evt.status == osEventMail)
	{
		queue_message_t *queue_msg_get;
		queue_msg_get = evt.value.p;
		char* token = strtok((char*)queue_msg_get->p_buffer, " ");
		COMMAND_findAndExecuteCommand(token);
		osMailFree(rxMailQueueHandle, queue_msg_get);
	}
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COM_uartPrint(char* token)
{
	queue_message_t *queue_msg_set;
	queue_msg_set = osMailAlloc(txMailQueueHandle, 0);
	if (queue_msg_set == NULL)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		return;
	}
	strncpy((char*)queue_msg_set->p_buffer, token, sizeof(queue_msg_set->p_buffer));
	osMailPut(txMailQueueHandle, queue_msg_set);
	if (!COM_getTxBusyFlag())
	{
		HAL_UART_TxCpltCallback(&huart2);
	}
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
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
		HAL_UART_Transmit_DMA(huart, Uart_Buffer.p_tx_buffer, strlen((char*)Uart_Buffer.p_tx_buffer));
		COM_setTxBusyFlagOn();
	}
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
static void COM_setTxBusyFlagOn(void)
{
	Uart_Buffer.tx_busy = true;
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COM_setTxBusyFlagOff(void)
{
	Uart_Buffer.tx_busy = false;
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
bool COM_getTxBusyFlag(void)
{
	return Uart_Buffer.tx_busy;
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
static void COM_bufferInit(uint8_t* p_buffer)
{
	memset(Uart_Buffer.p_rx_buffer, 0, sizeof(uint8_t) * Uart_Buffer.rx_index);
	Uart_Buffer.rx_index = 0;
}
