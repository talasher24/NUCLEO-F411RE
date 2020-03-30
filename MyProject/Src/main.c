/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "Types.h"
#include "Commands.h"
#include "MY_FLASH.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_TX_DMA
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CRC_Init(void);
static void MX_RTC_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void uart_print(char* token);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
s_Buff s_buffer;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CRC_Init();
  MX_RTC_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_DMA(&huart2, &s_buffer._rx_single_char, 1);

  memcpy((char*)s_buffer._p_tx_buffer, HELLO_WORLD, strlen(HELLO_WORLD));
#ifndef UART_TX_DMA
  HAL_UART_Transmit(&huart2, s_buffer._p_tx_buffer, strlen(HELLO_WORLD), 10);
#else
  HAL_UART_Transmit_DMA(&huart2, s_buffer._p_tx_buffer, strlen(HELLO_WORLD));
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	  HAL_Delay(1000);

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
    
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm A 
  */
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 1;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_ALL;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
    if (HAL_RTCEx_SetSmoothCalib(
		&hrtc,
		RTC_SMOOTHCALIB_PERIOD_32SEC,
		RTC_SMOOTHCALIB_PLUSPULSES_RESET,
		0x1FFU) != HAL_OK) {
    		Error_Handler();
      	}
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 84-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */

  //HAL_UART_Transmit_DMA(&huart2, s_buffer._p_tx_buffer, 20);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
	if (s_buffer._rx_single_char != '\n')
	{
		if(s_buffer._rx_index < BUFFER_SIZE)
		{
			s_buffer._p_rx_buffer[s_buffer._rx_index] = s_buffer._rx_single_char;
			s_buffer._rx_index++;
		}
	}
	else
	{
		s_buffer._p_rx_buffer[s_buffer._rx_index] = s_buffer._rx_single_char;
		s_buffer._rx_index++;
		whichCommand();
		bufferInit(&s_buffer);
		s_buffer._rx_index = 0;
	}

	//HAL_UART_Receive_IT(&huart2, &s_buffer._rx_single_char, 1);
	HAL_UART_Receive_DMA(&huart2, &s_buffer._rx_single_char, 1);
}

void whichCommand (void)
{
	char* token = strtok((char*)s_buffer._p_rx_buffer, " ");

	for (uint8_t i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if (strncmp(token, commands[i]._name, commands[i]._size)==0)
		{
			commands[i].func_ptr(token);
			return;
		}
	}
}

void ping_callBack(char* token)
{
/*#ifndef UART_TX_DMA
	HAL_UART_Transmit(&huart2, (uint8_t*)token, strlen(token), 10);
#else
	memcpy((char*)s_buffer._p_tx_buffer, (char*)token, strlen(token) +1);
	HAL_UART_Transmit_DMA(&huart2, s_buffer._p_tx_buffer, strlen(token));
#endif*/
	uart_print(token);
}

void version_callback(char* token)
{
  /*memcpy((char*)s_buffer._p_tx_buffer, VERSION, strlen(VERSION));
#ifndef UART_TX_DMA
  HAL_UART_Transmit(&huart2, s_buffer._p_tx_buffer, strlen(VERSION), 10);
#else
  HAL_UART_Transmit_DMA(&huart2, s_buffer._p_tx_buffer, strlen(VERSION));
#endif*/
  uart_print(VERSION);
}

void pwm_start_callback(char* token)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	/*memcpy((char*)s_buffer._p_tx_buffer, OK, strlen(OK));
#ifndef UART_TX_DMA
	HAL_UART_Transmit(&huart2, s_buffer._p_tx_buffer, strlen(OK), 10);
#else
	HAL_UART_Transmit_DMA(&huart2, s_buffer._p_tx_buffer, strlen(OK));
#endif*/
	uart_print(OK);
}

void pwm_stop_callback(char* token)
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	/*memcpy((char*)s_buffer._p_tx_buffer, OK, strlen(OK));
#ifndef UART_TX_DMA
	HAL_UART_Transmit(&huart2, s_buffer._p_tx_buffer, strlen(OK), 10);
#else
	HAL_UART_Transmit_DMA(&huart2, s_buffer._p_tx_buffer, strlen(OK));
#endif*/
	uart_print(OK);
}

void pwm_dc_callback(char* token)
{
	token = strtok(NULL, " ");

	if(strlen(token) <= 0)
	{
		return;
	}

	int32_t dc = atoi(token);

	if (dc > 100) dc = 100;
	if (dc < 0)   dc = 0;

	htim3.Instance->CCR1 = dc;

	/*memcpy((char*)s_buffer._p_tx_buffer, OK, strlen(OK));
#ifndef UART_TX_DMA
	HAL_UART_Transmit(&huart2, s_buffer._p_tx_buffer, strlen(OK), 10);
#else
	HAL_UART_Transmit_DMA(&huart2, s_buffer._p_tx_buffer, strlen(OK));
#endif*/
	uart_print(OK);
}

void crc_whole_flash_calc_callback(char* token)
{
	/*Flash information...
	uint16_t sector_0_Size = 4096; 				//16KB  - After multiplying by uint32_t
	uint16_t sector_1_Size = 4096; 				//16KB  - After multiplying by uint32_t
	uint16_t sector_2_Size = 4096; 				//16KB  - After multiplying by uint32_t
	uint16_t sector_3_Size = 4096; 				//16KB  - After multiplying by uint32_t
	uint16_t sector_4_Size = 4096 * 4; 			//64KB  - After multiplying by uint32_t
	uint16_t sector_5_Size = 4096 * 8;   		//128KB - After multiplying by uint32_t
	uint16_t sector_6_Size = 4096 * 8; 			//128KB - After multiplying by uint32_t
	uint16_t sector_7_Size = 4096 * 8; 			//128KB - After multiplying by uint32_t

	uint32_t sector_0_addr = 0x08000000;		//Sector 0 address
	uint32_t sector_1_addr = 0x08004000;		//Sector 1 address
	uint32_t sector_2_addr = 0x08008000;		//Sector 2 address
	uint32_t sector_3_addr = 0x0800C000;		//Sector 3 address
	uint32_t sector_4_addr = 0x08010000;		//Sector 4 address
	uint32_t sector_5_addr = 0x08020000;		//Sector 5 address
	uint32_t sector_6_addr = 0x08040000;		//Sector 6 address
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address


	//uint16_t sector_0_Size = 4096; 				//16KB
	//uint32_t sector_0_addr = 0x08000000;		//Sector 0 address
	//uint32_t flashDataBuffer[sector_0_Size];	//(4096(=0x1000) * 4Bytes(=sizeOf(int)) = 16384 (=0x4000))
	//uint32_t offsetAddr = 0x0;


	for(int i = 0; i < 32; i++)  				//(32 * 16384(=0x4000) = 524288(=0x80000)) => whole flash
	{
		MY_FLASH_SetSectorAddrs(0, sector_0_addr + offsetAddr);
		MY_FLASH_ReadN(0, flashDataBuffer, sector_0_Size, DATA_TYPE_32);
		crcFlashResult = HAL_CRC_Accumulate(&hcrc, flashDataBuffer, sector_0_Size);
		offsetAddr += 0x4000;
	}*/


	uint32_t crcFlashResult;
	uint32_t flashSize = 0x20000;
	uint32_t *p_flash_start_address = (uint32_t *) FLASH_START_ADDRESS;

	crcFlashResult = HAL_CRC_Calculate(&hcrc, p_flash_start_address, flashSize);

	sprintf((char*)s_buffer._p_tx_buffer, "%x\n", (unsigned int)crcFlashResult);

/*#ifndef UART_TX_DMA
	HAL_UART_Transmit(&huart2, (uint8_t*)s_buffer._p_tx_buffer, strlen((char*)s_buffer._p_tx_buffer), 10);
#else
	HAL_UART_Transmit_DMA(&huart2, s_buffer._p_tx_buffer, strlen((char*)s_buffer._p_tx_buffer));
#endif*/
	uart_print((char*)s_buffer._p_tx_buffer);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTC_AlarmAEventCallback could be implemented in the user file
   */
  /*memcpy((char*)s_buffer._p_tx_buffer, TICK, strlen(TICK));

#ifndef UART_TX_DMA
  HAL_UART_Transmit(&huart2, s_buffer._p_tx_buffer, strlen(TICK), 10);
#else
  HAL_UART_Transmit_DMA(&huart2, s_buffer._p_tx_buffer, strlen(TICK));
#endif*/

  uart_print(TICK);

}

void uart_print(char* token)
{
	memcpy((char*)s_buffer._p_tx_buffer, token, strlen(token));
#ifndef UART_TX_DMA
  HAL_UART_Transmit(&huart2, s_buffer._p_tx_buffer, strlen(token), 10);
#else
  HAL_UART_Transmit_DMA(&huart2, s_buffer._p_tx_buffer, strlen(token));
#endif
}

/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
