
#ifndef __Types_H__

#define __Types_H__

#define VERSION_SIZE 6

uint8_t m_p_version[VERSION_SIZE] = "0.0.4\n" ;

#define TICK_SIZE 5

uint8_t m_p_tick[TICK_SIZE] = "tick\n";


#define BUFFER_SIZE 20

typedef enum  {

e_buff_ready = 1,

e_buff_busy = 2
	
} t_m_buffer_state;

typedef struct {
	
	uint8_t _p_rx_buffer[BUFFER_SIZE];
	uint8_t _rx_index;
	uint8_t _single_char;
	t_m_buffer_state _state;
	
}s_Buff;

//void bufferInit(t_Buff buffer);

void bufferInit(s_Buff *buffer){
	for(int i = 0; i < BUFFER_SIZE; i++){
		buffer->_p_rx_buffer[i] = 0;
	}
}


#endif // __Types_H__

