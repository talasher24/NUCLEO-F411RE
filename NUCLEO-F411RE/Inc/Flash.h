/*
 * Flash.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

#ifndef MY_FLASH_H
#define MY_FLASH_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define FLASH_START_ADDRESS      0x08000000

typedef enum
{
	DATA_TYPE_8=0,
	DATA_TYPE_16,
	DATA_TYPE_32,
}DataTypeDef;
/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */

 //1. Erase Sector
 //static void FlashEraseSector(void); //uncomment this when erasing sector
 //2. Set Sector Adress
 void flashSetSectorAddress(uint8_t sector, uint32_t addrs);
 //3. Write Flash
 void flashWriteN(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType);
 //4. Read Flash
 void flashReadN(uint32_t idx, void *rdBuf, uint32_t Nsize, DataTypeDef dataType);
 //5. Write OTP
 void flashOtpWriteN(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType);

 HAL_StatusTypeDef flashWrpSectorEnable (void);

 HAL_StatusTypeDef flashWrpSectorDisable (void);

/* USER CODE END Prototypes */
#ifdef __cplusplus
}
#endif
#endif // MY_FLASH_H


