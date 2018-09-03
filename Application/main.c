// C99
#include <stdint.h>
#include <stdbool.h>

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
extern volatile ws_led_t BuffClock[LED_COUNT];

ws_led_t wsColorOff = { .RED=0, .GREEN=0, .BLUE=0 };

uint8_t brightness = 0;
uint16_t offset = 0;
uint8_t delayCnt = 0;
uint8_t delay = 2;
bool bPoweredOn = true;

void InitColors(void);
void CycleColors(void);

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

    InitColors();

    // Loop forever
    while (1)
    {

        CycleColors();
        delayCnt = delay;
        while(delayCnt > 1)
        {
            HAL_Delay(50);
            delayCnt--;
        }

        // Check for button press
        if( BUTTON_ReadHR() || BUTTON_ReadMIN() )
        {
            HAL_Delay(100);     // Delay some time for both button press

            if( BUTTON_ReadHR() && BUTTON_ReadMIN()  )
            {
                if(bPoweredOn)
                {
                    bPoweredOn = !bPoweredOn;
                    ClockSetColor(&wsColorOff);
                }
                else
                {
                    InitColors();
                }
                
            }
            else if( BUTTON_ReadHR() )
            {
                delay = (delay > 2) ? delay-1 : 1;
            }
            else if( BUTTON_ReadMIN() )
            {
                delay = (delay < 20) ? delay+1 : 20;
            }

            while(BUTTON_ReadHR() || BUTTON_ReadMIN());  // Wait for buttons to be released
            
        }


    }
}


void HAL_SYSTICK_Callback(void)
{
    miliseconds++;
}


void InitColors(void)
{
    uint8_t i = 0;
    ws_led_t randColor = {.RED = 0x00,       .GREEN = 0x00,         .BLUE = 0x00 };

    for(i=0; i<LED_COUNT; i++)
    {
        randColor = Color_Wheel(i);
        BuffClock[i].RED    = randColor.RED;
        BuffClock[i].GREEN  = randColor.GREEN;
        BuffClock[i].BLUE   = randColor.BLUE;
    }

    WS2812_send((ws_led_t *)&BuffClock[0], LED_COUNT);
}

void CycleColors(void)
{
    uint8_t i = 0;
    uint8_t j = 0;

    for(i=0; i<LED_COUNT; i++)
    {
        // Swap colors
        j = ((i+1) % (LED_COUNT-1));

        BuffClock[i].RED    = BuffClock[j].RED;
        BuffClock[i].GREEN  = BuffClock[j].GREEN;
        BuffClock[i].BLUE   = BuffClock[j].BLUE;
    }

    WS2812_send((ws_led_t *)&BuffClock[0], LED_COUNT);
}

