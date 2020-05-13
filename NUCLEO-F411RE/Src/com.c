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
	 uint8_t p_buffer[BUFFER_SIZE];
} queue_message_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

static osThreadId terminalTaskHandle;
static bool Tx_Busy;
static uint8_t Rx_Msg_Index;
static queue_message_t *P_Tx_Current_Msg;
static queue_message_t *P_Rx_Current_Msg;
static osMailQId  TxMailQueueHandle;
static osMailQId  RxMailQueueHandle;
static osMutexId UART_Tx_Mutex_Handle;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

static void COM_halUartReceiveDma(void);
static bool COM_charHandler(void);
static void COM_bufferInit(uint8_t* p_buffer);
static void COM_setTxBusyFlagOff(void);
static void COM_setTxBusyFlagOn(void);
static void COM_readyCommandProcess(char* p_buffer);

/******************************************************************************
* Function Definitions
*******************************************************************************/

/* USER CODE BEGIN Header_StartTerminalTask */
/**
* @brief Function implementing the terminalTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTerminalTask */
void StartTerminalTask(void const * argument)
{
  /* USER CODE BEGIN StartTerminalTask */
  /* Infinite loop */
  for(;;)
  {
	  osEvent evt;
	  evt = osMailGet(RxMailQueueHandle, osWaitForever);
	  if (evt.status == osEventMail)
	  {
		  queue_message_t *p_queue_msg_get;
		  p_queue_msg_get = evt.value.p;
		  COM_readyCommandProcess((char*)p_queue_msg_get->p_buffer);
		  osMailFree(RxMailQueueHandle, p_queue_msg_get);
	  }

  }
  /* USER CODE END StartTerminalTask */
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COM_init(void)
{
	/* definition and creation of terminalTask */
	osThreadDef(terminalTask, StartTerminalTask, osPriorityNormal, 0, 128);
	terminalTaskHandle = osThreadCreate(osThread(terminalTask), NULL);

	osMutexDef(UART_Tx_Mutex);
	UART_Tx_Mutex_Handle = osMutexCreate(osMutex(UART_Tx_Mutex));
	if (UART_Tx_Mutex_Handle == NULL)
	{
		Error_Handler();
		return;
	}

	osMailQDef(txMailQueue, 16, queue_message_t);
	TxMailQueueHandle = osMailCreate(osMailQ(txMailQueue), NULL);

	osMailQDef(rxMailQueue, 16, queue_message_t);
	RxMailQueueHandle = osMailCreate(osMailQ(rxMailQueue), NULL);

	P_Rx_Current_Msg = osMailAlloc(RxMailQueueHandle, 0);
	if (P_Rx_Current_Msg == NULL)
	{
		Error_Handler();
		return;
	}
	COM_bufferInit(P_Rx_Current_Msg->p_buffer);
	COM_halUartReceiveDma();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
static void COM_halUartReceiveDma(void)
{
	HAL_UART_Receive_DMA(&huart2, &(P_Rx_Current_Msg->p_buffer[Rx_Msg_Index]), 1);
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
	if (COM_charHandler())
	{
		osMailPut(RxMailQueueHandle, P_Rx_Current_Msg);
		P_Rx_Current_Msg = osMailAlloc(RxMailQueueHandle, 0);
		if (P_Rx_Current_Msg == NULL)
		{
			Error_Handler();
			return;
		}
		COM_bufferInit(P_Rx_Current_Msg->p_buffer);
	}
	COM_halUartReceiveDma();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
static bool COM_charHandler(void)
{
	if (P_Rx_Current_Msg->p_buffer[Rx_Msg_Index] != '\n')
	{
		if(Rx_Msg_Index < BUFFER_SIZE - 1)
		{
			Rx_Msg_Index++;
		}
	}
	else
	{
		P_Rx_Current_Msg->p_buffer[Rx_Msg_Index] = '\0';
		return true;
	}
	return false;
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
static void COM_readyCommandProcess(char* p_buffer)
{
	char* p_token = strtok(p_buffer, " ");
	COMMAND_findAndExecuteCommand(p_token);
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void COM_uartPrint(char* p_token)
{
	queue_message_t *p_queue_msg_set;
	p_queue_msg_set = osMailAlloc(TxMailQueueHandle, 0);
	if (p_queue_msg_set == NULL)
	{
		Error_Handler();
		return;
	}
	strncpy((char*)p_queue_msg_set->p_buffer, p_token, sizeof(p_queue_msg_set->p_buffer));
	osMailPut(TxMailQueueHandle, p_queue_msg_set);
	if (!COM_getTxBusyFlag() && osMutexWait(UART_Tx_Mutex_Handle, 0) == osOK)
	{
		HAL_UART_TxCpltCallback(&huart2);
		osMutexRelease(UART_Tx_Mutex_Handle);
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

	if (COM_getTxBusyFlag())
	{
		osMailFree(TxMailQueueHandle, P_Tx_Current_Msg);
	}

	osEvent evt;
	evt = osMailGet(TxMailQueueHandle, 0);
	if (evt.status != osEventMail)
	{
		COM_setTxBusyFlagOff();
	}
	else
	{
		P_Tx_Current_Msg = evt.value.p;
		HAL_UART_Transmit_DMA(huart, P_Tx_Current_Msg->p_buffer, strlen((char*)P_Tx_Current_Msg->p_buffer));
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
	Tx_Busy = true;
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
static void COM_setTxBusyFlagOff(void)
{
	Tx_Busy = false;
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
bool COM_getTxBusyFlag(void)
{
	return Tx_Busy;
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
static void COM_bufferInit(uint8_t* p_buffer)
{
	memset(p_buffer, 0, sizeof(uint8_t) * BUFFER_SIZE);
	Rx_Msg_Index = 0;
}
