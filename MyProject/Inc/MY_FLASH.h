
#ifndef __MY_FLASH_H__

#define __MY_FLASH_H__

#include "stm32f4xx_hal.h"

#define FLASH_START_ADDRESS      0x08000000

//Typedefs
//1. data size
typedef enum
{
	DATA_TYPE_8=0,
	DATA_TYPE_16,
	DATA_TYPE_32,
}DataTypeDef;

//functions prototypes
//1. Erase Sector
//static void MY_FLASH_EraseSector(void); //uncomment this when writing

//2. Set Sector Adress
void MY_FLASH_SetSectorAddrs(uint8_t sector, uint32_t addrs);
//3. Write Flash
void MY_FLASH_WriteN(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType);
//4. Read Flash
void MY_FLASH_ReadN(uint32_t idx, void *rdBuf, uint32_t Nsize, DataTypeDef dataType);
//5. Write OTP
void MY_FLASH_OTP_WriteN(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType);

HAL_StatusTypeDef WRP_sector_enable (void);

HAL_StatusTypeDef WRP_sector_disable (void);

#endif // __MY_FLASH_H__


