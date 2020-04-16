/*
 * Buffer.c
 *
 *  Created on: Apr 6, 2020
 *      Author: ADMIN
 */

#include "Buffer.h"
#include "main.h"


void BufferInit(uint8_t* rxBuffer)
{
	for(int i = 0; i < BUFFER_SIZE; i++){
		rxBuffer[i] = 0;
	}
}


