# Project Title
Working with NUCLEO-F411RE

## Description
### Topics used in the project:
Serial protocols:
```
I2C - Communicates with the LSM6DSL sensor
UART - Communicates with the PC
```
ST MCU peripherals:
```
DMA - Uses UART DMA
Timers - Uses GPIO to output PWM signal
RTC - Uses RTC to print “tick” every 1 second
CRC - Uses the HW CRC module to calculate the CRC of the whole flash
Internal flash - Uses read/write/erase, read/write protection
Watchdog - Uses IWDG to detect and recover from computer malfunctions
```
MEMS sensors:
```
LSM6DSL - Prints every sample to the terminal (acceleration + gyro values)
```

### Commands implemented in the project
The project executes the following commands received through serial terminal:
```
ping
get_version
pwm_start
pwm_dc XXX
pwm_stop
crc_whole_flash_calc
iwdg_test
flash_lock
set_SN
get_SN
start_tick
stop_tick
flash_lock
assert_0
clear_assert_flag
```

### Prerequisites
What things you need to install the software and how to install them
```
STM32CubeMX
STM32CubeIDE
STM32 ST-LINK Utility
Serial communication terminal 
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## Authors
* **Tal Asher**

