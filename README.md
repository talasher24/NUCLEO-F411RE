# Project Title
Working with NUCLEO-F411RE

## Description
### Topics used in the project:
Serial protocols:
```
I2C             - Communicates with the LSM6DSL sensor
UART            - Communicates with the PC
```
ST MCU peripherals:
```
DMA             - Uses UART DMA
Timers          - Uses GPIO to output PWM signal
RTC             - Uses RTC to print “tick” every 1 second
CRC             - Uses the HW CRC module to calculate the CRC of the whole flash
Internal flash  - Uses read/write/erase, read/write protection
                - Uses RAM to defines assertion struct that retains its value after reboot. Saves there the last command made.
                  The next reboot should print the value that the pointer pointed to.          
Watchdog        - Uses IWDG to detect and recover from computer malfunctions
Reset cause     - Prints reset cause upon startup
Low power mode  - Uses sleep, stop and standby mode for power saving
```
MEMS sensors:
```
LSM6DSL:
* Polling mode  - Prints every sample to the terminal (acceleration + gyro values)
* FIFO mode     - Generates an interrupt when it has 10 samples of acceleration and gyro each in the FIFO.
                  Upon every interrupt, reads all the data from the FIFO and prints the average.
```

### Commands implemented in the project
The project executes the following commands received through serial terminal:
```
ping                            - Returns ping
get_version                     - Returns SW version
pwm_start                       - Starts the PWM
pwm_dc XXX                      - Sets PWM value (0-100) 
pwm_stop                        - Stops the PWM
crc_whole_flash_calc            - Calculates the CRC of the whole flash and prints the result in hex format
iwdg_test                       - Tests the watchdog i.e. enters infinite loop
flash_lock                      - Locks the flash for reading
set_SN                          - Saves S/N in sector 7 - Linker file has been changed so the code won't overrun this sector
get_SN                          - Reads the S/N from sector 7
start_tick                      - Prints “tick” every 1 second using RTC
stop_tick                       - Stops “tick” printing                      
assert_0                        - Tests the assertion struct by generating assert(0)
clear_assert_flag               - Resets assertion struct flag 
lsm6dsl_per_sample_enable       - Enables Polling mode
lsm6dsl_fifo_enable             - Enables FIFO modede
lsm6dsl_disable                 - Disables Both Polling and FIFO mode
enter_stop_mode X               - Puts the system into stop mode, system wakes up using RTC (X sec) or GPIO_EXT (PC13, LSM6DSL, etc.)
enter_standby_mode              - Puts the system into stop mode, system wakes up using RTC (5 sec) or SYS_WKUP pin (PA0)
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

