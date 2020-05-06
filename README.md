# Project Title
Working with NUCLEO-F411RE

## Description
### Topics used in the project:
Serial protocols:
```
I2C             	- Used for communicating with the LSM6DSL sensor
UART            	- Used for communicating with the PC
```
ST MCU peripherals:
```
DMA             	- Used as one of the UART methods
Timers          	- Used for generating PWM output signal using a GPIO
RTC             	- Used for printing “tick” every 1 second
CRC             	- Used for calculating the whole flash CRC
Internal flash		- Used for locking the flash for reading
					- Used for saving a serial number
RAM					- Used for defining assertion struct that retains its value after reboot, saves there the last command made and prints the value in the next reboot.
IWDG        		- Used for detecting and recovering a computer malfunctions
Reset cause     	- Used for printing reset cause upon startup
Low power mode  	- Used for power saving - sleep, stop, and standby modes are used
```
MEMS sensors:
```
LSM6DSL				- Used for sampling acceleration and angular velocity
```

FreeRTOS:
```
Default task		- Responsible for interrupts received from the terminal and the LSM6DSL using READY_COMMAND_SIGNAL and LSM6DSL_SIGNAL
Signal Events		- Used for triggering execution states between tasks and interrupts
Timer Management	- Used for printing text to the terminal every X seconds
Mail Queues:		- Used for Exchanging data between threads using a queue of memory blocks
* txMailQueue		- Used for managing outgoing prints
```

### Commands implemented in the project
The project executes the following commands received through serial terminal:
```
ping                            - Returns ping
get_version                     - Returns SW version
pwm_start                       - Starts the PWM
pwm_dc X	                    - Sets PWM value, the range of X is between 0 and 100
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
lsm6dsl_per_sample_enable       - Prints every sample to the terminal (acceleration + gyro values)
lsm6dsl_fifo_enable             - Generates an interrupt when it has 10 samples of acceleration and gyro each in the FIFO
								  Upon every interrupt, reads all the data from the FIFO and prints the average
lsm6dsl_disable                 - Disables Both per_sample and FIFO modes
enter_stop_mode X               - Puts the system into stop mode, system wakes up using RTC (X sec) or GPIO_EXT (PC13, LSM6DSL, etc.)
enter_standby_mode              - Puts the system into stop mode, system wakes up using RTC (5 sec) or SYS_WKUP pin (PA0)
start_os_timer X				- Starts the osTimer, a message will be printed to the terminal every X seconds
stop_os_timer					- Stops the osTimer
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

