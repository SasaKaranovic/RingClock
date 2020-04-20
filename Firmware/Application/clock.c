#include "main.h"
#include <math.h>
#include "clock_config.h"
#include "clock.h"
#include "rtc.h"
#include "ws2812b.h"

// Clock Constants
#define SILENT_ON_VALUE                     0x64

const ws_led_t COLOR_CLOCK_OFF              = { .RED = 0x00,        .GREEN = 0x00,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_MARKER           = { .RED = 0xd8,        .GREEN = 0x6c,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_MARKER_SILENT    = { .RED = 0x64,        .GREEN = 0x64,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_MINUTE           = { .RED = 0x00,        .GREEN = 0x20,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_MINUTE_SILENT    = { .RED = 0x00,        .GREEN = 0x64,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_HOUR             = { .RED = 0xF0,        .GREEN = 0x00,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_HOUR_SILENT      = { .RED = 0x64,        .GREEN = 0x00,         .BLUE = 0x00 };

// Clock related variables
volatile ws_led_t BuffClock[LED_COUNT]      = { {.RED = 0x00,       .GREEN = 0x00,         .BLUE = 0x00 } };
float LED_Brightness                        = DIM_LED_IS_ON;

// Fade in-out helper variables
volatile ws_led_t colorMinutes_current;
static uint8_t clockPhase = 62;
static clockMode_t clockMode        = ModeRainbowDynamic;
static clockMode_t clockModeWhenOn  = ModeRainbowDynamic;

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
        if( clockMode != ModeSilent)
        {
            led = (i + LED_OFFSET) % 60;
            BuffClock[led] = COLOR_CLOCK_MARKER;
        }
        else
        {
            led = (i + LED_OFFSET) % 60;
            BuffClock[led] = COLOR_CLOCK_MARKER_SILENT;
        }
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

    // Between 19:00 (DIM_HOUR_TURN_ON) and 21:00 (DIM_HOUR_TURN_MAX), set LED brightness to 7% (DIM_LED_IS_DIM)
    if( (ClockHour >= DIM_HOUR_TURN_ON) && (ClockHour < DIM_HOUR_TURN_MAX) )
    {
        LED_Brightness = DIM_LED_IS_DIM;
        clockMode = clockModeWhenOn;
    }
    // Between 21:00 (DIM_TURN_ON) and 07:00 (DIM_HOUR_TURN_OFF), set LED brightness to 0.5% (DIM_LED_IS_DIM)
    else if ( (ClockHour >= DIM_HOUR_TURN_MAX) ||  (ClockHour < DIM_HOUR_TURN_OFF) )
    {
        LED_Brightness = DIM_LED_IS_MAX;
        clockMode = ModeSilent;
    }

    // Between 07:00 (DIM_HOUR_TURN_OFF) and 19:00 (DIM_HOUR_TURN_ON), set LED brightness to 15% (DIM_LED_IS_ON)
    else 
    {
        LED_Brightness = DIM_LED_IS_ON;
        clockMode = clockModeWhenOn;
    }
    Clock_DimAll();
    Clock_SendData();
}


void Clock_DrawTime(uint8_t ClockHour, uint8_t ClockMinute)
{
    uint8_t led, ledMin, ledHour;

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
    
    clockPhase = (clockPhase > 0) ? --clockPhase : 62;

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
        else if (clockMode == ModeSilent)
        {
            colorMinutes_current = COLOR_CLOCK_OFF;
        }

        led = (i + LED_OFFSET) % 60;
        if( (led%5) != 0) //Don't change colors of hour markers
        {   
            BuffClock[led] = colorMinutes_current;
        }
    }


    //As last step, draw hour and minute as solid colors
    ledMin      = (ClockMinute + LED_OFFSET) % 60;
    ledHour     = ((ClockHour*5) + LED_OFFSET) % 60;

    if(clockMode != ModeSilent)
    {
        // Set current MINUTE LED
        BuffClock[ledMin]      = COLOR_CLOCK_MINUTE;
        // Set current HOUR LED
        BuffClock[ledHour]      = COLOR_CLOCK_HOUR;
    }
    else
    {
        // Set current MINUTE LED
        BuffClock[ledMin]      = COLOR_CLOCK_MINUTE_SILENT;
        // Set current HOUR LED
        BuffClock[ledHour]      = COLOR_CLOCK_HOUR_SILENT;
    }

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
        if(clockMode != ModeSilent)
        {
            BuffClock[i].RED     = (BuffClock[i].RED     * LED_Brightness);
            BuffClock[i].GREEN   = (BuffClock[i].GREEN   * LED_Brightness);
            BuffClock[i].BLUE    = (BuffClock[i].BLUE    * LED_Brightness);
        }
        else
        {
            BuffClock[i].RED     = (BuffClock[i].RED > 0)       ? 1 : 0;
            BuffClock[i].GREEN   = (BuffClock[i].GREEN > 0)     ? 1 : 0;
            BuffClock[i].BLUE    = (BuffClock[i].BLUE > 0)      ? 1 : 0;
        }
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
    if ( (SetMode_t > Mode_LAST) )
    {
        debug_log("Invalid '0x%02X' mode value! \r\n", SetMode_t);
        return;
    }

    clockModeWhenOn = SetMode_t;
}


clockMode_t Clock_GetMode(void)
{
    return clockMode;
}