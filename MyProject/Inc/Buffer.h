#ifndef Buffer_H__

#define Buffer_H__



#define BUFFER_SIZE 25

typedef struct {
	uint8_t _p_rx_buffer[BUFFER_SIZE];
	uint8_t _rx_index;
	uint8_t _rx_single_char;
	uint8_t _p_tx_buffer[BUFFER_SIZE];
}s_Buff;

void bufferInit(s_Buff *buffer);

void bufferInit(s_Buff *buffer){
	for(int i = 0; i < BUFFER_SIZE; i++){
		buffer->_p_rx_buffer[i] = 0;
	}
}


#endif // Buffer_H__
