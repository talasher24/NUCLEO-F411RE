/*
 * system_isr.h
 *
 *  Created on: Mar 26, 2020
 *      Author: Tal Asher
 */

#ifndef SYSTEM_ISR_H
#define SYSTEM_ISR_H
#ifdef __cplusplus
 extern "C" {
#endif

 /******************************************************************************
 * Includes
 *******************************************************************************/

 /******************************************************************************
 * Module Preprocessor Constants
 *******************************************************************************/

#define LSM6DSL_SIGNAL   		0x01

 /******************************************************************************
 * Module Preprocessor Macros
 *******************************************************************************/

 /******************************************************************************
 * Module Typedefs
 *******************************************************************************/

 /******************************************************************************
 * Module Variable Definitions
 *******************************************************************************/

 /******************************************************************************
 * Function Prototypes
 *******************************************************************************/

 void SYSTEM_ISR_init(void);
 void SYSTEM_ISR_osTimerStart(uint32_t osTimer01_time);
 void SYSTEM_ISR_osTimerStop(void);

 /******************************************************************************
 * Function Definitions
 *******************************************************************************/


#ifdef __cplusplus
}
#endif
#endif // SYSTEM_ISR_H

