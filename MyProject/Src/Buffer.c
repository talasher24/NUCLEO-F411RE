/*
 * Buffer.c
 *
 *  Created on: Apr 6, 2020
 *      Author: ADMIN
 */

#include "Buffer.h"
#include "main.h"


void bufferInit(s_Buff *buffer)
{
	for(int i = 0; i < BUFFER_SIZE; i++){
		buffer->_p_rx_buffer[i] = 0;
	}
}


