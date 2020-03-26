
#ifndef __Types_H__

#define __Types_H__

#define BUFFER_SIZE 20

#define PING "ping"

typedef enum  {

e_buff_ready = 1,

e_buff_busy = 2
	
} t_m_buffer_state;

typedef struct {
	
	uint8_t m_p_rx_buffer[BUFFER_SIZE];
	uint8_t m_rx_index;
	uint8_t m_single_char;
	t_m_buffer_state m_state;
	
}s_Buff;

//void bufferInit(t_Buff buffer);

void bufferInit(s_Buff *buffer){
	for(int i = 0; i < BUFFER_SIZE; i++){
		buffer->m_p_rx_buffer[i] = 0;
	}
}

typedef struct {
	
	uint8_t m_p_name[15];
	uint8_t m_size;
	
}s_Command;

typedef struct {
	
	s_Command m_ping;
	s_Command m_version;
	
}s_Commands;


#endif // __Types_H__

