// C99
#include <stdint.h>

// HAL
#include "stm32f1xx.h"


// App
#include "sys_init.h"
#include "clock_config.h"
#include "clock.h"
#include "debug.h"
#include "rtc.h"

#define refreshColor        2000
#define refreshDBGTime      60000

// Clock variables
uint8_t clockHours              = 0;
uint8_t clockMinutes            = 0;
uint8_t clockSeconds            = 0;

// Various variables
volatile uint32_t miliseconds   = 0;
volatile uint32_t updateTime    = 0;
volatile uint32_t updateLED     = 0;

extern RTC_HandleTypeDef hrtc;

uint8_t brightness = 0;


int main(void)
{
    Sys_Init();

    DebugLED_On();
    UserLED_On();

    debug_log("WallClock Up&Running\r\n");
    RTC_ReadTime(&clockHours, &clockMinutes, &clockSeconds);
    HAL_Delay(500);
    DebugLED_Off();
    UserLED_Off();

    // Loop forever
    while (1){

        // Update LEDs and cycle colors
        if(miliseconds >= updateLED)
        {
            DebugLED_On();
            Clock_Update(clockHours, clockMinutes);
            updateLED = miliseconds + refreshColor;
            DebugLED_Off();
        }

        // Read time from RTC and update local variables
        if(miliseconds >= updateTime)
        {
            RTC_ReadTime(&clockHours, &clockMinutes, &clockSeconds);
            debug("Debug: %02d:%02d:%02d", clockHours, clockMinutes, clockSeconds);
            updateTime = miliseconds + refreshDBGTime;
        }

        // Read Hour/Minute butons
        while(BUTTON_ReadHR() || BUTTON_ReadMIN())
        {
            UserLED_On();
            //Increment hours
            if(BUTTON_ReadHR())
            {
                clockHours = (clockHours < 23) ? ++clockHours : 0;
                RTC_SetTime(clockHours, clockMinutes, 0);
            }
            else if(BUTTON_ReadMIN())
            {
                clockMinutes = (clockMinutes < 59) ? ++clockMinutes : 0;
                RTC_SetTime(clockHours, clockMinutes, 0);
            }

            // Update LEDs
            Clock_Update(clockHours, clockMinutes);
            
            //Wait for both buttons to be released
            while(BUTTON_ReadHR() || BUTTON_ReadMIN());

            //Software debounce
            HAL_Delay(150);
            UserLED_Off();

        }

    }
}


void HAL_SYSTICK_Callback(void)
{
    miliseconds++;
}


