
#ifndef Debug_H__

#define Debug_H__


/*------------#define------------*/

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define ASSERT_FLAG_ON 0xAA

#define ASSERT_FLAG_OFF 0x55

bool int1_occurred;


typedef struct {
	char _file [60];
	uint32_t _line;
	uint8_t flag;
}assert_struct;

typedef enum reset_cause_t
{
    RESET_CAUSE_UNKNOWN = 0,
    RESET_CAUSE_LOW_POWER_RESET,
    RESET_CAUSE_WINDOW_WATCHDOG_RESET,
    RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
    RESET_CAUSE_SOFTWARE_RESET,
    RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
    RESET_CAUSE_EXTERNAL_RESET_PIN_RESET,
    RESET_CAUSE_BROWNOUT_RESET,
} reset_cause_t;

reset_cause_t reset_cause_get(void);
const char * reset_cause_get_name(reset_cause_t reset_cause);


#endif // Debug_H__
