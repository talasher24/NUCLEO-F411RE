/*
 * Commands.c
 *
 *  Created on: Apr 5, 2020
 *      Author: ADMIN
 */



#include "Commands.h"
#include "main.h"
#include "MY_FLASH.h"
#include "Buffer.h"
#include "Types.h"



#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include <stdbool.h>


 extern TIM_HandleTypeDef htim3;
 extern CRC_HandleTypeDef hcrc;
 extern RTC_HandleTypeDef hrtc;

 extern void MX_RTC_Init(void);
 extern void uart_print(char* token);
 extern s_Buff s_uart_buffer;
 extern assert_struct s_assert_struct;



void ping_callBack(char* token)
{
	uart_print(token);
}

void version_callback(char* token)
{
	uart_print(VERSION);
}

void pwm_start_callback(char* token)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void pwm_stop_callback(char* token)
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
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

	char temp [9];
	uint32_t crcFlashResult;
	uint32_t flashSize = 0x20000;
	uint32_t *p_flash_start_address = (uint32_t *) FLASH_START_ADDRESS;

	crcFlashResult = HAL_CRC_Calculate(&hcrc, p_flash_start_address, flashSize);

	sprintf(temp, "%x\n", (unsigned int)crcFlashResult);
	uart_print(temp);
	/*
	sprintf((char*)s_uart_buffer._p_tx_buffer, "%x\n", (unsigned int)crcFlashResult);
	uart_print((char*)s_uart_buffer._p_tx_buffer);
	*/
}

void iwdg_test_callback(char* token)
{
	uart_print(OK);
	while(1);
}

void flash_lock_callback(char* token)
{

	/*RDP protects the whole internal flash from reading from outside, via debugger interface.
	 *It does not prevent one part of code to read another part, or even rewrite.
	 *PCROP allows you to use debugger to debug your code,
	 *but protects reading and rewriting the "secret" part.*/

	FLASH_OBProgramInitTypeDef obConfig;
	HAL_FLASHEx_OBGetConfig(&obConfig);

	if (obConfig.RDPLevel == OB_RDP_LEVEL_0) {
		obConfig.RDPLevel = OB_RDP_LEVEL_1;
		obConfig.OptionType = OPTIONBYTE_RDP;

		if (HAL_FLASH_Unlock() != HAL_OK)
		{
			Error_Handler();
		}
		/* Clear All pending flags */ //if wwdg and kickDog are enabled - uncomment __HAL_FLASH_CLEAR_FLAG
		//__HAL_FLASH_CLEAR_FLAG (FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
		if (HAL_FLASH_OB_Unlock() != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_FLASHEx_OBProgram(&obConfig) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_FLASH_OB_Launch() != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_FLASH_OB_Lock() != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_FLASH_Lock() != HAL_OK)
		{
			Error_Handler();
		}

		/*
		//DEBUG - use for writing data to flash
		uint32_t sector_7_addr       = 0x08060000;		//Sector 7 address
		MY_FLASH_SetSectorAddrs(7, sector_7_addr);
		uint32_t myTestWrite[1] = {0xDEADBEEF}; 		//0xFFFFFFFF, 0xDEADBEEF
		MY_FLASH_WriteN(131072-4, myTestWrite2, 1, DATA_TYPE_32);
		*/
	}
}

void set_SN_callback(char* token)
{
	/*if (WRP_sector_disable() != HAL_OK)
	{
		Error_Handler();
	}*/
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	MY_FLASH_SetSectorAddrs(7, sector_7_addr);
	uint32_t myTestWrite[1] = {0xDEADBEEF}; //0xFFFFFFFF, 0xDEADBEEF
	MY_FLASH_WriteN(0, myTestWrite, 1, DATA_TYPE_32);

	/*if (WRP_sector_enable() != HAL_OK)
	{
		Error_Handler();
	}*/
}

void get_SN_callback(char* token)
{
	char temp [9];
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	MY_FLASH_SetSectorAddrs(7, sector_7_addr);
	uint32_t myTestRead[1];
	MY_FLASH_ReadN(0, myTestRead, 1, DATA_TYPE_32);

	sprintf(temp, "%x\n", (unsigned int)myTestRead[0]);
	uart_print(temp);
	/*
	sprintf((char*)s_uart_buffer._p_tx_buffer, "%x\n", (unsigned int)myTestRead[0]);
	uart_print((char*)s_uart_buffer._p_tx_buffer);
	*/
}

void start_tick_callback(char* token)
{
	MX_RTC_Init();
}

void stop_tick_callback(char* token)
{
	if (HAL_RTC_DeInit(&hrtc) != HAL_OK)
	{
		Error_Handler();
	}
}

void assert_0_callback(char* token)
{
	assert_param(0);
}

void clear_assert_flag_callback(char* token)
{
	s_assert_struct.flag = ASSERT_FLAG_OFF;
}
