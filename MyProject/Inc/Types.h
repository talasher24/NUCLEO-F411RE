
#ifndef Types_H__

#define Types_H__


#include <stdbool.h>

/*------------#define------------*/

#define VERSION "0.0.18\n"

#define HELLO_WORLD "Hello World\n"

#define TICK "tick\n"

#define OK "OK\n"

typedef struct {
	char _file [60];
	uint32_t _line;
	bool flag;
}assert_struct;

//__attribute__((section('.noinit')))

//__attribute__((section(".noinit")));

//__attribute__((section(".my_section"))) assert_struct s_assert_struct;

//__attribute__((section(".my_section"))) uint32_t a;

#endif // Types_H__

