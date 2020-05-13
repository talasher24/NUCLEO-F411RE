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

 /******************************************************************************
 * Module Preprocessor Constants
 *******************************************************************************/

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
 } data_type_e;

 /******************************************************************************
 * Module Variable Definitions
 *******************************************************************************/

 /******************************************************************************
 * Function Prototypes
 *******************************************************************************/

 void FLASH_setSectorAddress(uint8_t sector, uint32_t addrs);
 void FLASH_writeN(uint32_t idx, void *p_wrBuf, uint32_t size, data_type_e dataType);
 void FLASH_readN(uint32_t idx, void *p_rdBuf, uint32_t size, data_type_e dataType);
 void FLASH_wrpSectorEnable (void);
 void FLASH_wrpSectorDisable (void);
 void FLASH_crcWholeFlashCalc(void);
 void FLASH_lock(void);
 void FLASH_setSerialNumberInSector7(void);
 void FLASH_getSerialNumberFromSector7(void);

 /******************************************************************************
 * Function Definitions
 *******************************************************************************/


#ifdef __cplusplus
}
#endif
#endif // MY_FLASH_H

