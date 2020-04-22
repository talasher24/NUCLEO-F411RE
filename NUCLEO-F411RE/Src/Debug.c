

#include <Buffer.h>
#include <Debug.h>
#include <main.h>
#include <stm32f411xe.h>
#include <stm32f4xx.h>
#include <stm32f4xx_hal_def.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_iwdg.h>
#include <stm32f4xx_hal_pwr.h>
#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_rtc.h>
#include <stm32f4xx_hal_rtc_ex.h>



extern IWDG_HandleTypeDef hiwdg;
extern RTC_HandleTypeDef hrtc;
extern void SystemClock_Config(void);

extern s_Buff s_uart_buffer;

extern void uart_print(char* token);


#ifdef IWDG_ENABLE
void kickDog(void)
{
	HAL_IWDG_Refresh(&hiwdg);
}
#endif

reset_cause_t reset_cause_get(void)
{
    reset_cause_t reset_cause;

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
    {
        reset_cause = RESET_CAUSE_LOW_POWER_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
    {
        reset_cause = RESET_CAUSE_WINDOW_WATCHDOG_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
    {
        reset_cause = RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
    {
        reset_cause = RESET_CAUSE_SOFTWARE_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
    {
        reset_cause = RESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
    {
        reset_cause = RESET_CAUSE_EXTERNAL_RESET_PIN_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
    {
        reset_cause = RESET_CAUSE_BROWNOUT_RESET;
    }
    else
    {
        reset_cause = RESET_CAUSE_UNKNOWN;
    }

    __HAL_RCC_CLEAR_RESET_FLAGS();

    return reset_cause;
}


const char * reset_cause_get_name(reset_cause_t reset_cause)
{
    const char * reset_cause_name = "TBD";

    switch (reset_cause)
    {
        case RESET_CAUSE_UNKNOWN:
            reset_cause_name = "UNKNOWN";
            break;
        case RESET_CAUSE_LOW_POWER_RESET:
            reset_cause_name = "LOW_POWER_RESET";
            break;
        case RESET_CAUSE_WINDOW_WATCHDOG_RESET:
            reset_cause_name = "WINDOW_WATCHDOG_RESET";
            break;
        case RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET:
            reset_cause_name = "INDEPENDENT_WATCHDOG_RESET";
            break;
        case RESET_CAUSE_SOFTWARE_RESET:
            reset_cause_name = "SOFTWARE_RESET";
            break;
        case RESET_CAUSE_POWER_ON_POWER_DOWN_RESET:
            reset_cause_name = "POWER-ON_RESET (POR) / POWER-DOWN_RESET (PDR)";
            break;
        case RESET_CAUSE_EXTERNAL_RESET_PIN_RESET:
            reset_cause_name = "EXTERNAL_RESET_PIN_RESET";
            break;
        case RESET_CAUSE_BROWNOUT_RESET:
            reset_cause_name = "BROWNOUT_RESET (BOR)";
            break;
    }

    return reset_cause_name;
}

void enter_sleep_mode(void)
{
	//uart_print("SLEEP MODE is ON\n");
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	while (s_uart_buffer.tx_busy);

	// Enters to sleep mode
	HAL_SuspendTick();
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	HAL_ResumeTick();

	//uart_print("SLEEP MODE is OFF\n");
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	//while (s_uart_buffer.tx_busy);
}

void enter_stop_mode(void)
{

	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	uart_print("STOP MODE is ON\n");

	/* enable the RTC Wakeup */
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x2710, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	{
		Error_Handler();
	}

	// Enters to stop mode
	while (s_uart_buffer.tx_busy);
	HAL_SuspendTick();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

	SystemClock_Config();
	HAL_ResumeTick();

	/** Deactivate the RTC wakeup  **/
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	uart_print("STOP MODE is OFF\n");

}

void wakeup_stop_mode(void)
{
	SystemClock_Config();
	HAL_ResumeTick();

	/** Deactivate the RTC wakeup  **/
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
}

void enter_standby_mode(void)
{
	/* Clear the WU FLAG */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	 /* clear the RTC Wake UP (WU) flag */
	 __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

	 /* Enable the WAKEUP PIN */
	 HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	 /* enable the RTC Wakeup */
	    /*  RTC Wake-up Interrupt Generation:
	      Wake-up Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
	      ==> WakeUpCounter = Wake-up Time / Wake-up Time Base

	      To configure the wake up timer to 5s the WakeUpCounter is set to 0x2710:
	      RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
	      Wake-up Time Base = 16 /(32KHz) = 0.0005 seconds
	      ==> WakeUpCounter = ~5s/0.0005s = 20000 = 0x2710
	    */
	 if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x2710, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	 {
		 Error_Handler();
	 }
	 uart_print("STANDBY MODE is ON\n");

	 /* Enter the standby mode */
	 HAL_PWR_EnterSTANDBYMode();
}

void wakeup_standby_mode(void)
{
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);  // clear the flag

		uart_print("Wakeup from STANDBY MODE\n");

		/** Blink the LED **/
		for (int i=0; i<20; i++)
		{
		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		  HAL_Delay(200);
		}

		/** Disable the WWAKEUP PIN **/
		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);  // disable PA0

		/** Deactivate the RTC wakeup  **/
		HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	}
}


