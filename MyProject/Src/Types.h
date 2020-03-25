
#ifndef __Types_H__

#define __Types_H__

uint8_t m_p_version[5] = 	"0.0.2" ;

#define BUFFER_SIZE 20

/*------------Commands------------*/

#define PING "ping"

#define PING_SIZE 4

#define GET_VERSION "get_version"

#define GET_VERSION_SIZE 5

#define TICK "tick\n"

#define TICK_SIZE 5

/*--------End_of_Commands--------*/

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

typedef struct {
	
	uint8_t _p_name[15];
	uint8_t _size;
	
}s_Command;

typedef struct {
	
	s_Command _ping;
	s_Command _version;
	s_Command _tick;
	
}s_Commands;


#endif // __Types_H__

