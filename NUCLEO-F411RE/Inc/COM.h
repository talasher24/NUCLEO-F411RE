#ifndef COM_H

#define COM_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define BUFFER_SIZE 100

typedef struct {
	uint8_t _p_rx_buffer[BUFFER_SIZE];
	uint8_t _rx_index;
	uint8_t _rx_single_char;
	bool _rx_ready_command;
	uint8_t _p_tx_buffer[BUFFER_SIZE];
	bool tx_busy;
}s_Buff;


void readyCommandProcess(void);
void COM_whichCommand(void);
void uartPrint(char* token);


void halUartReceiveDma(void);
void txBusyFlagEnable(void);
void txBusyFlagDisable(void);
bool getTxBusyFlag(void);


void charHandler(void);

void bufferInit(uint8_t* rxBuffer);



#endif // COM_H
