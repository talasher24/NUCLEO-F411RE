/*
 * Commands.c
 *
 *  Created on: Apr 5, 2020
 *      Author: ADMIN
 */


/*
#include "Commands.h"
#include "main.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>*/
//#include "stm32f4xx_hal.h"

/*
 extern void uart_print(char* token);
 extern TIM_HandleTypeDef htim3;

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
	uart_print(OK);
}

void pwm_stop_callback(char* token)
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
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
	uart_print(OK);
}

void crc_whole_flash_calc_callback(char* token)
{
	uint32_t crcFlashResult;
	uint32_t flashSize = 0x20000;
	uint32_t *p_flash_start_address = (uint32_t *) FLASH_START_ADDRESS;
	//FeedDog(1);
	crcFlashResult = HAL_CRC_Calculate(&hcrc, p_flash_start_address, flashSize);
	//FeedDog(1);
	sprintf((char*)s_buffer._p_tx_buffer, "%x\n", (unsigned int)crcFlashResult);
	uart_print((char*)s_buffer._p_tx_buffer);
}

void wwdg_test_callback(char* token)
{
	uart_print(OK);
	while(1);
}

void flash_lock_callback(char* token)
{

	//  RDP protects the whole internal flash from reading from outside, via debugger interface.
	// *It does not prevent one part of code to read another part, or even rewrite.
	// *PCROP allows you to use debugger to debug your code,
	// *but protects reading and rewriting the "secret" part.

	FLASH_OBProgramInitTypeDef obConfig;
	HAL_FLASHEx_OBGetConfig(&obConfig);

	if (obConfig.RDPLevel == OB_RDP_LEVEL_0) {
		obConfig.RDPLevel = OB_RDP_LEVEL_1;
		obConfig.OptionType = OPTIONBYTE_RDP;

		if (HAL_FLASH_Unlock() != HAL_OK)
		{
			Error_Handler();
		}
		// Clear All pending flags  //if wwdg and kickDog are enabled - uncomment __HAL_FLASH_CLEAR_FLAG
		////__HAL_FLASH_CLEAR_FLAG (FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
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
		uart_print(OK);



		////DEBUG - use for writing data to flash
		//uint32_t sector_7_addr       = 0x08060000;		//Sector 7 address
		//MY_FLASH_SetSectorAddrs(7, sector_7_addr);
		//uint32_t myTestWrite[1] = {0xDEADBEEF}; 		//0xFFFFFFFF, 0xDEADBEEF
		//MY_FLASH_WriteN(131072-4, myTestWrite2, 1, DATA_TYPE_32);

	}
}

void set_SN_callback(char* token)
{
	if (WRP_sector_disable() != HAL_OK)
	{
		Error_Handler();
	}
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	MY_FLASH_SetSectorAddrs(7, sector_7_addr);
	uint32_t myTestWrite[1] = {0xDEADBEEF}; //0xFFFFFFFF, 0xDEADBEEF
	MY_FLASH_WriteN(0, myTestWrite, 1, DATA_TYPE_32);

	if (WRP_sector_enable() != HAL_OK)
	{
		Error_Handler();
	}
}

void get_SN_callback(char* token)
{
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	MY_FLASH_SetSectorAddrs(7, sector_7_addr);
	uint32_t myTestRead[1];
	MY_FLASH_ReadN(0, myTestRead, 1, DATA_TYPE_32);
	sprintf((char*)s_buffer._p_tx_buffer, "%x\n", (unsigned int)myTestRead[0]);
	uart_print((char*)s_buffer._p_tx_buffer);
}

*/

