// C99
#include <stdint.h>

// HAL
#include "stm32f1xx.h"


// App
#include "sys_init.h"
#include "clock.h"
#include "debug.h"
#include "rtc.h"

#define refreshColor    100
#define refreshTime     200

// Clock variables
uint8_t clockHours              = 0;
uint8_t clockMinutes            = 0;
uint8_t clockSeconds            = 0;

// Various variables
volatile uint32_t miliseconds   = 0;
volatile uint32_t updateTime    = 0;
volatile uint32_t updateLED     = 0;

uint8_t brightness = 0;


int main(void)
{
    Sys_Init();

    // Test
    RTC_SetTime(1, 10, 0);

    debug_log("WallClock Up&Running\r\n");

    // Loop forever
    while (1){

        // Update LEDs and cycle colors
        if(miliseconds >= updateLED)
        {
            Clock_Clear();
            Clock_DrawHands();
            Clock_DrawTime(clockHours, clockMinutes);
            Clock_SendData();
            updateLED = miliseconds + refreshColor;
        }

        // Read time from RTC and update local variables
        if(miliseconds >= updateTime)
        {
            RTC_ReadTime(&clockHours, &clockMinutes, &clockSeconds);
            debug("Read back time: %02d:%02d:%02d", clockHours, clockMinutes, clockSeconds);
            updateTime = miliseconds + refreshTime;
        }

        // Read Hour/Minute butons
        while(BUTTON_ReadHR() || BUTTON_ReadMIN())
        {
            //Increment hours
            if(BUTTON_ReadHR())
            {
                clockHours++;
                RTC_SetTime(clockHours, clockMinutes, 0);
            }
            else if(BUTTON_ReadMIN())
            {
                clockMinutes++;
                RTC_SetTime(clockHours, clockMinutes, 0);
            }

            //Software debounce
            HAL_Delay(300);

            //Wait for both buttons to be released
            while(BUTTON_ReadHR() || BUTTON_ReadMIN());
        }

    }
}


void HAL_SYSTICK_Callback(void)
{
    miliseconds++;
}
