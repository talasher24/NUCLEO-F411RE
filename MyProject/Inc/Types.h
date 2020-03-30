
#ifndef __Types_H__

#define __Types_H__

#define VERSION_SIZE 7

uint8_t m_p_version[VERSION_SIZE] = "0.0.10\n" ;

#define m_p_startup_data_length 12

uint8_t m_p_startup_data[m_p_startup_data_length] = "Hello World\n";

char* newline = "\n";

#define OK_SIZE 3

uint8_t m_p_ok[OK_SIZE] = "OK\n" ;

#define TICK_SIZE 5

uint8_t m_p_tick[TICK_SIZE] = "tick\n";


#define BUFFER_SIZE 25


typedef struct {
	
	uint8_t _p_rx_buffer[BUFFER_SIZE];
	uint8_t _rx_index;
	uint8_t _rx_single_char;
	uint8_t _p_tx_buffer[BUFFER_SIZE];
	uint8_t _tx_index;
	uint8_t _tx_single_char;
}s_Buff;

//void bufferInit(t_Buff buffer);

void bufferInit(s_Buff *buffer){
	for(int i = 0; i < BUFFER_SIZE; i++){
		buffer->_p_rx_buffer[i] = 0;
	}
}


#endif // __Types_H__

