#include "main.h"
#include <math.h>
#include "clock_config.h"
#include "clock.h"
#include "rtc.h"
#include "ws2812b.h"

// Clock Constants
const ws_led_t COLOR_CLOCK_OFF              = { .RED = 0x00,        .GREEN = 0x00,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_MARKER           = { .RED = 0xd8,        .GREEN = 0x6c,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_MINUTE           = { .RED = 0x00,        .GREEN = 0x20,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_HOUR             = { .RED = 0xF0,        .GREEN = 0x00,         .BLUE = 0x00 };

// Clock related variables
volatile ws_led_t BuffClock[LED_COUNT]      = { {.RED = 0x00,       .GREEN = 0x00,         .BLUE = 0x00 } };
float LED_Brightness                        = DIM_LED_IS_ON;

// Fade in-out helper variables
volatile ws_led_t colorMinutes_current;
static uint8_t clockPhase = 0;
static clockMode_t clockMode = ModeSameColor;

// Color fade states
uint16_t colorFadeState = 0;


// Local functions
static void Clock_DimAll(void);


void Clock_Clear(void)
{
    // Set all LEDs to Off (0x00)
    memset((void *)BuffClock, 0, LED_COUNT * sizeof(struct ws_led_t));
}

void ClockSetColor(ws_led_t *color)
{
    uint8_t i;
    for(i=0; i<LED_COUNT; i++)
    {
        BuffClock[i].RED    = color->RED;
        BuffClock[i].GREEN  = color->GREEN;
        BuffClock[i].BLUE   = color->BLUE;
    }
}

void Clock_DrawHands(void)
{
    uint8_t i, led;
    for(i=0; i<LED_COUNT; i+=5)
    {
        led = (i + LED_OFFSET) % 60;
        BuffClock[led] = COLOR_CLOCK_MARKER;
    }
}

ws_led_t Color_Wheel(uint16_t count)
{
    ws_led_t ColorWheel;

    ColorWheel.RED   = (uint8_t) (sin(0.1*count + 0) * 127 + 128);
    ColorWheel.GREEN = (uint8_t) (sin(0.1*count + 2) * 127 + 128);
    ColorWheel.BLUE  = (uint8_t) (sin(0.1*count + 4) * 127 + 128);

    return ColorWheel;
}

void Clock_Update(uint8_t ClockHour, uint8_t ClockMinute)
{
    Clock_Clear();
    Clock_DrawHands();
    Clock_DrawTime(ClockHour, ClockMinute);

    // Check if dimming is enabled and dim LEDs if needed
    if( (ClockHour >= DIM_HOUR_TURN_ON) || (ClockHour < DIM_HOUR_TURN_OFF) )
    {
        LED_Brightness = DIM_LED_IS_DIM;
    }
    else 
    {
        LED_Brightness = DIM_LED_IS_ON;
    }
    Clock_DimAll();
    Clock_SendData();
}


void Clock_DrawTime(uint8_t ClockHour, uint8_t ClockMinute)
{
    uint8_t led;

    if(ClockMinute >= 60)
    {
        ClockMinute = 0;
    }
    if(ClockHour >= 12)
    {
        ClockHour -= 12;
        if(ClockHour >= 12)
        {
            ClockHour = 0;
        }
    }

    // Get next color from our color wheel
    colorMinutes_current = Color_Wheel(colorFadeState);
    colorFadeState = (colorFadeState < 60) ? ++colorFadeState : 0;
    
    clockPhase += 1;

    //Mark elapsed minutes with newely chosen color
    for (uint8_t i = 0; i < ClockMinute; ++i)
    {
        if(clockMode == ModeRainbowStatic)
        {
            colorMinutes_current = Color_Wheel(i);
        }
        else if(clockMode == ModeRainbowDynamic)
        {
            colorMinutes_current = Color_Wheel(i+clockPhase);
        }

        led = (i + LED_OFFSET) % 60;
        if( (led%5) != 0) //Don't change colors of hour markers
        {   
            BuffClock[led] = colorMinutes_current;
        }
    }


    //As last step, draw hour and minute as solid colors
    // Set current MINUTE LED
    led = (ClockMinute + LED_OFFSET) % 60;
    BuffClock[led]      = COLOR_CLOCK_MINUTE;

    // Set current HOUR LED
    led = ((ClockHour*5) + LED_OFFSET) % 60;
    BuffClock[led]      = COLOR_CLOCK_HOUR;
}

void Clock_SendData(void)
{
    WS2812_send((ws_led_t *)&BuffClock[0], LED_COUNT);
}


static void Clock_DimAll(void)
{
    uint8_t i;

    // Dim all LEDs
    // Not most eficient way but easy
    for(i=0; i<LED_COUNT; i++)
    {
        BuffClock[i].RED     = (BuffClock[i].RED     * LED_Brightness);
        BuffClock[i].GREEN   = (BuffClock[i].GREEN   * LED_Brightness);
        BuffClock[i].BLUE    = (BuffClock[i].BLUE    * LED_Brightness);
    }
}

uint8_t Clock_GetBrightness(void)
{
    return LED_Brightness;
}

void Clock_SetBrightness(uint8_t value)
{
    LED_Brightness = value;
}

void Clock_SetMode(clockMode_t SetMode_t)
{
    if ( (SetMode_t > Mode_LAST) || (SetMode_t > Mode_LAST) )
    {
        debug_log("Invalid '0x%02X' mode value! \r\n", SetMode_t);
        return;
    }

    clockMode = SetMode_t;
}


clockMode_t Clock_GetMode(void)
{
    return clockMode;
}