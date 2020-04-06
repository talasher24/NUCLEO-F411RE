#ifndef Buffer_H__

#define Buffer_H__

#include "stm32f4xx_hal.h"

#define BUFFER_SIZE 25

typedef struct {
	uint8_t _p_rx_buffer[BUFFER_SIZE];
	uint8_t _rx_index;
	uint8_t _rx_single_char;
	uint8_t _p_tx_buffer[BUFFER_SIZE];
}s_Buff;

void bufferInit(s_Buff *buffer);



#endif // Buffer_H__
