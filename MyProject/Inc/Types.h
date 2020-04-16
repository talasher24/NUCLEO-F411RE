
#ifndef Types_H__

#define Types_H__


#include <stdbool.h>

/*------------#define------------*/

#define VERSION "0.0.24\n"

#define HELLO_WORLD "Hello World\n"

#define TICK "tick\n"

#define OK "OK\n"

#define ASSERT_FLAG_ON 0xAA

#define ASSERT_FLAG_OFF 0x55

bool int1_occurred;

typedef struct {
	char _file [60];
	uint32_t _line;
	uint8_t flag;
}assert_struct;

#endif // Types_H__

