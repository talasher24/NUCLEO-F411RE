
#include "Flash.h"
#include "main.h"
#include "COM.h"


//Private variables
//1. sector start address
static uint32_t MY_SectorAddrs;
static uint8_t MY_SectorNum;

//functions definitions
//1. Erase Sector
static void FlashEraseSector(void)
{
	HAL_FLASH_Unlock();
	//Erase the required Flash sector
	FLASH_Erase_Sector(MY_SectorNum, FLASH_VOLTAGE_RANGE_3);
	HAL_FLASH_Lock();
}

//2. Set Sector Adress
void flashSetSectorAddress(uint8_t sector, uint32_t addrs)
{
	MY_SectorNum = sector;
	MY_SectorAddrs = addrs;
}

//3. Write Flash
void flashWriteN(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType)
{
	uint32_t flashAddress = MY_SectorAddrs + idx;

	//Erase sector before write
	FlashEraseSector();

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
//4. Read Flash
void flashReadN(uint32_t idx, void *rdBuf, uint32_t Nsize, DataTypeDef dataType)
{
	uint32_t flashAddress = MY_SectorAddrs + idx;

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

//5. Write OTP
void flashOtpWriteN(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType)
{
	uint32_t flashAddress = MY_SectorAddrs + idx;

	//Erase sector before write
	//FlashEraseSector();

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

HAL_StatusTypeDef flashWrpSectorEnable (void)
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
			status = HAL_OK; //uartPrint("wrp enabled\n");
		}
		else
		{
		  uartPrint("wrp not enabled\n");
		}
	}
	else
	{
		status = HAL_OK; //uartPrint("wrp is already enabled\n");
	}
	return status;
}

HAL_StatusTypeDef flashWrpSectorDisable (void)
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
			status = HAL_OK; //uartPrint("wrp disabled\n");
		}
		else
		{
		  uartPrint("wrp not disabled\n");
		}
	}
	else
	{
		status = HAL_OK; //uartPrint("wrp is already disabled\n");
	}
	return status;
}

