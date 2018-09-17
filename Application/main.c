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

#define periodReadRTC                   20000

// Clock variables
uint8_t clockHours                      = 0;
uint8_t clockMinutes                    = 0;
uint8_t clockSeconds                    = 0;

// Various variables
volatile uint32_t miliseconds           = 0;
volatile uint32_t updateTime            = 0;
volatile uint32_t updateLED             = 0;
volatile uint32_t periodUpdateClock     = LED_UPDATE_SPEED_SLOW;

extern RTC_HandleTypeDef hrtc;

uint8_t brightness = 0;


int main(void)
{
    Sys_Init();

    DebugLED_On();
    UserLED_On();

    debug_log("WallClock Up&Running\r\n");
    // RTC_SetTime(20, 50, 0);
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
            updateLED = miliseconds + periodUpdateClock        DebugLED_Off();
        }

        // Read time from RTC and update local variables
        if(miliseconds >= updateTime)
        {
            RTC_ReadTime(&clockHours, &clockMinutes, &clockSeconds);
            debug("Debug: %02d:%02d:%02d", clockHours, clockMinutes, clockSeconds);
            updateTime = miliseconds + periodReadRTC;
        }

        // Read Hour/Minute butons
        while(BUTTON_ReadHR() || BUTTON_ReadMIN())
        {
            // Wait about 200ms to detect both button pressed event. Also act as SWd debounce
            UserLED_On();
            HAL_Delay(150);

            if( BUTTON_ReadHR() && BUTTON_ReadMIN() )
            {
                switch(Clock_GetMode())
                {
                    case ModeSameColor:
                        Clock_SetMode(ModeRainbowStatic);
                        periodUpdateClock = LED_UPDATE_SPEED_SLOW;
                    break;

                    case ModeRainbowStatic:
                        Clock_SetMode(ModeRainbowDynamic);
                        periodUpdateClock = LED_UPDATE_SPEED_FAST;
                    break;

                    case ModeRainbowDynamic:
                        Clock_SetMode(ModeSameColor);
                        periodUpdateClock = LED_UPDATE_SPEED_SLOW;
                    break;

                    default:
                        Clock_SetMode(ModeSameColor);
                        periodUpdateClock = LED_UPDATE_SPEED_SLOW;
                    break;
                }
            }
            else
            {
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
                UserLED_Off();
                
            }

        }

    }
}


void HAL_SYSTICK_Callback(void)
{
    miliseconds++;
}


