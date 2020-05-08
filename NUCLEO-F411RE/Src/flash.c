/*
 * flash.c
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

/******************************************************************************
* Includes
*******************************************************************************/

#include "main.h"
#include "flash.h"
#include "com.h"
#include "crc.h"
#include <stdio.h>

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

#define FLASH_START_ADDRESS      0x08000000

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

static uint32_t Sector_Addrs;
static uint8_t Sector_Num;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

static void FLASH_eraseSector(void);

/******************************************************************************
* Function Definitions
*******************************************************************************/

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
static void FLASH_eraseSector(void)
{
	HAL_FLASH_Unlock();
	//Erase the required Flash sector
	FLASH_Erase_Sector(Sector_Num, FLASH_VOLTAGE_RANGE_3);
	HAL_FLASH_Lock();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void FLASH_setSectorAddress(uint8_t sector, uint32_t addrs)
{
	Sector_Num = sector;
	Sector_Addrs = addrs;
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void FLASH_writeN(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType)
{
	uint32_t flashAddress = Sector_Addrs + idx;

	//Erase sector before write
	FLASH_eraseSector();

	//Unlock Flash
	HAL_FLASH_Unlock();
	//Write to Flash
	switch(dataType)
	{
		case DATA_TYPE_8:
				for(uint32_t i=0; i<Nsize; i++)
				{
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, flashAddress , ((uint8_t *)wrBuf)[i]);
					flashAddress++;
				}
			break;

		case DATA_TYPE_16:
				for(uint32_t i=0; i<Nsize; i++)
				{
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, flashAddress , ((uint16_t *)wrBuf)[i]);
					flashAddress+=2;
				}
			break;

		case DATA_TYPE_32:
				for(uint32_t i=0; i<Nsize; i++)
				{
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddress , ((uint32_t *)wrBuf)[i]);
					flashAddress+=4;
				}
			break;
	}
	//Lock the Flash space
	HAL_FLASH_Lock();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void FLASH_readN(uint32_t idx, void *rdBuf, uint32_t Nsize, DataTypeDef dataType)
{
	uint32_t flashAddress = Sector_Addrs + idx;

	switch(dataType)
	{
		case DATA_TYPE_8:
				for(uint32_t i=0; i<Nsize; i++)
				{
					*((uint8_t *)rdBuf + i) = *(uint8_t *)flashAddress;
					flashAddress++;
				}
			break;

		case DATA_TYPE_16:
				for(uint32_t i=0; i<Nsize; i++)
				{
					*((uint16_t *)rdBuf + i) = *(uint16_t *)flashAddress;
					flashAddress+=2;
				}
			break;

		case DATA_TYPE_32:
				for(uint32_t i=0; i<Nsize; i++)
				{
					*((uint32_t *)rdBuf + i) = *(uint32_t *)flashAddress;
					flashAddress+=4;
				}
			break;
	}
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void FLASH_wrpSectorEnable (void)
{
	/* Private define ------------------------------------------------------------*/
	#define FLASH_WRP_SECTORS   (/*OB_WRP_SECTOR_6 |*/ OB_WRP_SECTOR_7) /* sectors 6 and 7  */
	/* Private variables ---------------------------------------------------------*/
	FLASH_OBProgramInitTypeDef OBInit;
	__IO uint32_t SectorsWRPStatus = 0xFFF;

	/* Get FLASH_WRP_SECTORS write protection status */
	HAL_FLASHEx_OBGetConfig(&OBInit);
	SectorsWRPStatus = OBInit.WRPSector & FLASH_WRP_SECTORS;

	if (SectorsWRPStatus != 0)
	{
		/* If FLASH_WRP_SECTORS are not write protected, enable the write protection */

		/* Allow Access to option bytes sector */
		HAL_FLASH_OB_Unlock();

		/* Allow Access to Flash control registers and user Flash */
		HAL_FLASH_Unlock();

		/* Enable FLASH_WRP_SECTORS write protection */
		OBInit.OptionType = OPTIONBYTE_WRP;
		OBInit.WRPState   = WRPSTATE_ENABLE;
		OBInit.WRPSector  = FLASH_WRP_SECTORS;
		HAL_FLASHEx_OBProgram(&OBInit);

		/* Start the Option Bytes programming process */
		if (HAL_FLASH_OB_Launch() != HAL_OK)
		{
		/* User can add here some code to deal with this error */
		  Error_Handler();
		}

		/* Prevent Access to option bytes sector */
		HAL_FLASH_OB_Lock();

		/* Disable the Flash option control register access (recommended to protect
		the option Bytes against possible unwanted operations) */
		HAL_FLASH_Lock();

		/* Get FLASH_WRP_SECTORS write protection status */
		HAL_FLASHEx_OBGetConfig(&OBInit);
		SectorsWRPStatus = OBInit.WRPSector & FLASH_WRP_SECTORS;

		/* Check if FLASH_WRP_SECTORS are write protected */
		if (SectorsWRPStatus == 0)
		{
			//COM_uartPrint("wrp enabled\n");
		}
		else
		{
		  COM_uartPrint("wrp not enabled\n");
		}
	}
	else
	{
		//COM_uartPrint("wrp is already enabled\n");
	}
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void FLASH_wrpSectorDisable (void)
{
	/* Private define ------------------------------------------------------------*/
	#define FLASH_WRP_SECTORS   (/*OB_WRP_SECTOR_6 |*/ OB_WRP_SECTOR_7) /* sectors 6 and 7  */
	/* Private variables ---------------------------------------------------------*/
	FLASH_OBProgramInitTypeDef OBInit;
	__IO uint32_t SectorsWRPStatus = 0xFFF;

	/* Get FLASH_WRP_SECTORS write protection status */
	HAL_FLASHEx_OBGetConfig(&OBInit);
	SectorsWRPStatus = OBInit.WRPSector & FLASH_WRP_SECTORS;

	if (SectorsWRPStatus == 0)
	{
		/* If FLASH_WRP_SECTORS are write protected, disable the write protection */

		/* Allow Access to option bytes sector */
		HAL_FLASH_OB_Unlock();

		/* Allow Access to Flash control registers and user Flash */
		HAL_FLASH_Unlock();

		/* Disable FLASH_WRP_SECTORS write protection */
		OBInit.OptionType = OPTIONBYTE_WRP;
		OBInit.WRPState   = WRPSTATE_DISABLE;
		OBInit.WRPSector  = FLASH_WRP_SECTORS;
		HAL_FLASHEx_OBProgram(&OBInit);

		/* Start the Option Bytes programming process */
		if (HAL_FLASH_OB_Launch() != HAL_OK)
		{
		/* User can add here some code to deal with this error */
		  Error_Handler();
		}

		/* Prevent Access to option bytes sector */
		HAL_FLASH_OB_Lock();

		/* Disable the Flash option control register access (recommended to protect
		the option Bytes against possible unwanted operations) */
		HAL_FLASH_Lock();

		/* Get FLASH_WRP_SECTORS write protection status */
		HAL_FLASHEx_OBGetConfig(&OBInit);
		SectorsWRPStatus = OBInit.WRPSector & FLASH_WRP_SECTORS;

		/* Check if FLASH_WRP_SECTORS write protection is disabled */
		if (SectorsWRPStatus == FLASH_WRP_SECTORS)
		{
			//COM_uartPrint("wrp disabled\n");
		}
		else
		{
		  COM_uartPrint("wrp not disabled\n");
		}
	}
	else
	{
		//COM_uartPrint("wrp is already disabled\n");
	}
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void FLASH_crcWholeFlashCalc(void)
{
	char temp [9];
	uint32_t crcFlashResult;
	uint32_t flashSize = 0x20000;
	uint32_t *p_flash_start_address = (uint32_t *) FLASH_START_ADDRESS;

	crcFlashResult = HAL_CRC_Calculate(&hcrc, p_flash_start_address, flashSize);

	sprintf(temp, "%x\n", (unsigned int)crcFlashResult);
	COM_uartPrint(temp);
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void FLASH_lock(void)
{
	/*RDP protects the whole internal flash from reading from outside, via debugger interface.
	 *It does not prevent one part of code to read another part, or even rewrite.
	 *PCROP allows you to use debugger to debug your code,
	 *but protects reading and rewriting the "secret" part.*/

	FLASH_OBProgramInitTypeDef obConfig;
	HAL_FLASHEx_OBGetConfig(&obConfig);

	if (obConfig.RDPLevel == OB_RDP_LEVEL_0)
	{
		obConfig.RDPLevel = OB_RDP_LEVEL_1;
		obConfig.OptionType = OPTIONBYTE_RDP;

		if (HAL_FLASH_Unlock() != HAL_OK)
		{
			Error_Handler();
		}
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
		FLASH_setSectorAddress(7, sector_7_addr);
		uint32_t myTestWrite[1] = {0xDEADBEEF}; 		//0xFFFFFFFF, 0xDEADBEEF
		FLASH_writeN(131072-4, myTestWrite2, 1, DATA_TYPE_32);
		*/
	}
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void FLASH_setSerialNumberInSector7(void)
{
	//if (FLASH_wrpSectorDisable();

	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	FLASH_setSectorAddress(7, sector_7_addr);
	uint32_t myTestWrite[1] = {0xDEADBEEF}; //0xFFFFFFFF, 0xDEADBEEF
	FLASH_writeN(0, myTestWrite, 1, DATA_TYPE_32);

	//FLASH_wrpSectorEnable();
}

/**
  * @brief 	XXX
  * @param 	XXX
  * @retval	XXX
  */
void FLASH_getSerialNumberFromSector7(void)
{
	char temp [9];
	uint32_t sector_7_addr = 0x08060000;		//Sector 7 address
	FLASH_setSectorAddress(7, sector_7_addr);
	uint32_t myTestRead[1];
	FLASH_readN(0, myTestRead, 1, DATA_TYPE_32);

	sprintf(temp, "%x\n", (unsigned int)myTestRead[0]);
	COM_uartPrint(temp);
}
