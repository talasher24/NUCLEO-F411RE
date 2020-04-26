/*
 * flash.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

#ifndef MY_FLASH_H
#define MY_FLASH_H
#ifdef __cplusplus
 extern "C" {
#endif

 /******************************************************************************
 * Includes
 *******************************************************************************/

#include "stm32f4xx_hal.h"

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

 typedef enum {
 	DATA_TYPE_8 = 0,
 	DATA_TYPE_16,
 	DATA_TYPE_32,
 } DataTypeDef;

 /******************************************************************************
 * Module Variable Definitions
 *******************************************************************************/

 /******************************************************************************
 * Function Prototypes
 *******************************************************************************/

 void FLASH_setSectorAddress(uint8_t sector, uint32_t addrs);
 void FLASH_writeN(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType);
 void FLASH_readN(uint32_t idx, void *rdBuf, uint32_t Nsize, DataTypeDef dataType);
 HAL_StatusTypeDef FLASH_wrpSectorEnable (void);
 HAL_StatusTypeDef FLASH_wrpSectorDisable (void);

 /******************************************************************************
 * Function Definitions
 *******************************************************************************/


#ifdef __cplusplus
}
#endif
#endif // MY_FLASH_H


