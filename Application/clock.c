#include "main.h"
#include <math.h>
#include "clock.h"
#include "rtc.h"
#include "ws2812b.h"

// Clock related hard-coded values
#define CLOCK_LED_MAX       60      // Number of LEDs we have 
#define CLOCK_LED_OFFSET    0      // Counting from "12hr" LED clock-wise, where is the first LED (one that gets DIN)
#define DIM_HOUR_ON         19      // Turn ON dim feature at 19:00
#define DIM_HOUR_OFF        8       // Turn OFF dim feature at 08:00
#define DIM_BRIGHTNESS_ON   200     // Dim to 30% of full intensity
#define DIM_BRIGHTNESS_OFF  10     // Dim to 80% of full intensity

// Macros and conveniance
#define ABS(value)  ( (value) >=0 ? (value) : -(value) )

// Clock Constants
const ws_led_t COLOR_CLOCK_OFF                  = { .RED = 0x00,        .GREEN = 0x00,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_MARKER               = { .RED = 0xd8,        .GREEN = 0x6c,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_MINUTE               = { .RED = 0x00,        .GREEN = 0x10,         .BLUE = 0x00 };
const ws_led_t COLOR_CLOCK_MINUTE_ELAPSED       = { .RED = 0x12,        .GREEN = 0x20,         .BLUE = 0x70 };
const ws_led_t COLOR_CLOCK_HOUR                 = { .RED = 0xF0,        .GREEN = 0x00,         .BLUE = 0x00 };

// Clock related variables
volatile ws_led_t BuffClock[CLOCK_LED_MAX]      = { {.RED = 0x00,        .GREEN = 0x00,         .BLUE = 0x00 } };   // Buffer for holding each LED value
volatile uint16_t ClockColorCycle               = 0;                               // 
uint8_t LED_Brightness                          = DIM_BRIGHTNESS_OFF;

// Fade in-out helper variables
volatile ws_led_t colorMinutes_target;
volatile ws_led_t colorMinutes_current;
volatile ws_led_t colorMinutes_increments;
uint8_t colorMinutes_fadeInSteps = 4;


// Local functions
static void Clock_DimAll(ws_led_t *buffer);


void Clock_Clear(void)
{
    uint8_t i;
    for(i=0; i<CLOCK_LED_MAX; i++)
    {
        BuffClock[i].RED    = 0;
        BuffClock[i].GREEN  = 0;
        BuffClock[i].BLUE   = 0;
    }
}

void ClockSetColor(ws_led_t *color)
{
    uint8_t i;
    for(i=0; i<CLOCK_LED_MAX; i++)
    {
        BuffClock[i].RED    = color->RED;
        BuffClock[i].GREEN  = color->GREEN;
        BuffClock[i].BLUE   = color->BLUE;
    }
}

void Clock_DrawHands(void)
{
    uint8_t i, led;
    for(i=0; i<CLOCK_LED_MAX; i+=5)
    {
        led = (i + CLOCK_LED_OFFSET) % 60;
        BuffClock[led] = COLOR_CLOCK_MARKER;
    }
}

ws_led_t Color_Wheel(uint16_t count)
{
    ws_led_t ColorWheel;

    uint8_t phase    = 0;
    double frequency = 3.14159265*2/60;
    ColorWheel.RED   = (uint8_t) (sin(frequency*count+2+phase) * 128 + 15);
    ColorWheel.GREEN = (uint8_t) (sin(frequency*count+0+phase) * 128 + 15);
    ColorWheel.BLUE  = (uint8_t) (sin(frequency*count+4+phase) * 128 + 15);

    return ColorWheel;
}


//Fade current color to target color by specified increments
//When target color is reached, returns 1; otherwise returns 0
uint8_t Color_FadeTo(ws_led_t *fadeThis, ws_led_t *targetColor, ws_led_t *inc)
{
    //Check if colors are already same
    if((fadeThis->RED == targetColor->RED) && (fadeThis->GREEN == targetColor->GREEN) && (fadeThis->BLUE == targetColor->BLUE))
    {
        return 1;
    }

    //Fade RED
    if(fadeThis->RED < targetColor->RED)
    {
        fadeThis->RED    = ((fadeThis->RED + inc->RED) >= targetColor->RED)   ? targetColor->RED   : (fadeThis->RED + inc->RED);
    }
    else 
    {
        fadeThis->RED    = ((fadeThis->RED - inc->RED) < targetColor->RED)   ? targetColor->RED   : (fadeThis->RED - inc->RED);
    }

    //Fade GREEN
    if(fadeThis->GREEN < targetColor->GREEN)
    {
        fadeThis->GREEN    = ((fadeThis->GREEN + inc->GREEN) >= targetColor->GREEN)   ? targetColor->GREEN   : (fadeThis->GREEN + inc->GREEN);
    }
    else 
    {
        fadeThis->GREEN    = ((fadeThis->GREEN - inc->GREEN) < targetColor->GREEN)   ? targetColor->GREEN   : (fadeThis->GREEN - inc->GREEN);
    }

    //Fade BLUE
    if(fadeThis->BLUE < targetColor->BLUE)
    {
        fadeThis->BLUE    = ((fadeThis->BLUE + inc->BLUE) >= targetColor->BLUE)   ? targetColor->BLUE   : (fadeThis->BLUE + inc->BLUE);
    }
    else 
    {
        fadeThis->BLUE    = ((fadeThis->BLUE - inc->BLUE) < targetColor->BLUE)   ? targetColor->BLUE   : (fadeThis->BLUE - inc->BLUE);
    }

    return 0;
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

    //If we have faded in to a target color, get a new color
    if(Color_FadeTo((ws_led_t *)&colorMinutes_current, (ws_led_t *)&colorMinutes_target, (ws_led_t *)&colorMinutes_increments))
    {
        ++ClockColorCycle;
        colorMinutes_target = Color_Wheel(ClockColorCycle);

        //Calculate step increment values
        colorMinutes_increments.RED = ABS(colorMinutes_current.RED - colorMinutes_target.RED) / colorMinutes_fadeInSteps;
        colorMinutes_increments.GREEN = ABS(colorMinutes_current.GREEN - colorMinutes_target.GREEN) / colorMinutes_fadeInSteps;
        colorMinutes_increments.BLUE = ABS(colorMinutes_current.BLUE - colorMinutes_target.BLUE) / colorMinutes_fadeInSteps;

        //Prevent increment steps from being zero
        if(colorMinutes_increments.RED == 0)    { colorMinutes_increments.RED = 1; }
        if(colorMinutes_increments.GREEN == 0)  { colorMinutes_increments.GREEN = 1; }
        if(colorMinutes_increments.BLUE == 0)   { colorMinutes_increments.BLUE = 1; }

    }

    //Mark elapsed minutes
    for (uint8_t i = 0; i < ClockMinute; ++i)
    {
        led = (i + CLOCK_LED_OFFSET) % 60;
        if( (led%5) != 0) //Don't change colors of hour markers
        {   
            BuffClock[led] = colorMinutes_current;
        }
    }


    //As last step, draw hour and minute as solid colors

    //SET Hour and Minute LEDs
    led = (ClockMinute + CLOCK_LED_OFFSET) % 60;
    BuffClock[led]      = COLOR_CLOCK_MINUTE;

    led = ((ClockHour*5) + CLOCK_LED_OFFSET) % 60;
    BuffClock[led]      = COLOR_CLOCK_HOUR;
}

void Clock_SendData(void)
{
    Clock_DimAll((ws_led_t *)&BuffClock[0]);
    WS2812_send((ws_led_t *)&BuffClock[0], CLOCK_LED_MAX);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}


static void Clock_DimAll(ws_led_t *buffer)
{
    // Invalid pointers check
    if(buffer == NULL)
    {
        debug("Invalid *buffer pointer!");
        return;
    }

    uint8_t i;

    // // Determine if we are withing hours that we should dim the LEDs
    // if( (clockHours >= DIM_HOUR_ON) || (clockHours < DIM_HOUR_OFF) )
    // {
    //     LED_Brightness = DIM_BRIGHTNESS_ON;
    // }
    // else
    // {
    //     // LED_Brightness = DIM_BRIGHTNESS_OFF;
    //     return;
    // }

    // Dim all LEDs
    // Not most eficient way but easy
    for(i=0; i<CLOCK_LED_MAX; i++)
    {
        buffer[i].RED     = (uint8_t)((buffer[i].RED     * LED_Brightness) >> 8);
        buffer[i].GREEN   = (uint8_t)((buffer[i].GREEN   * LED_Brightness) >> 8);
        buffer[i].BLUE    = (uint8_t)((buffer[i].BLUE    * LED_Brightness) >> 8);
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