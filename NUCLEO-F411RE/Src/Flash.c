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

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

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

static void FLASH_eraseSector(void)
{
	HAL_FLASH_Unlock();
	//Erase the required Flash sector
	FLASH_Erase_Sector(Sector_Num, FLASH_VOLTAGE_RANGE_3);
	HAL_FLASH_Lock();
}

void FLASH_setSectorAddress(uint8_t sector, uint32_t addrs)
{
	Sector_Num = sector;
	Sector_Addrs = addrs;
}

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

HAL_StatusTypeDef FLASH_wrpSectorEnable (void)
{
	HAL_StatusTypeDef status = HAL_ERROR;

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
			status = HAL_OK; //COM_uartPrint("wrp enabled\n");
		}
		else
		{
		  COM_uartPrint("wrp not enabled\n");
		}
	}
	else
	{
		status = HAL_OK; //COM_uartPrint("wrp is already enabled\n");
	}
	return status;
}

HAL_StatusTypeDef FLASH_wrpSectorDisable (void)
{
	HAL_StatusTypeDef status = HAL_ERROR;

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
			status = HAL_OK; //COM_uartPrint("wrp disabled\n");
		}
		else
		{
		  COM_uartPrint("wrp not disabled\n");
		}
	}
	else
	{
		status = HAL_OK; //COM_uartPrint("wrp is already disabled\n");
	}
	return status;
}

