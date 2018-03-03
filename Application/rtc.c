// C99
#include <stdint.h>

// HAL
#include <stm32f1xx.h>
#include <stm32f1xx_hal_rtc.h>

// App
#include "rtc.h"

// RTC config
RTC_HandleTypeDef hrtc =
{
    .Instance = RTC,
    .Init.AsynchPrediv = RTC_AUTO_1_SECOND,
    .Init.OutPut = RTC_OUTPUTSOURCE_ALARM
};

void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    RTC_TimeTypeDef sTime;
    sTime.Hours     = hours;
    sTime.Minutes   = minutes;
    sTime.Seconds   = seconds;

    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}


void RTC_ReadTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    // Invalid pointers check
    if(hours == NULL)
    {
        debug("Invalid *hours pointer!");
        return;
    }
    if(minutes == NULL)
    {
        debug("Invalid *minutes pointer!");
        return;
    }
    if(seconds == NULL)
    {
        debug("Invalid *seconds pointer!");
        return;
    }

    RTC_TimeTypeDef sTime;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    //debug("Read back time: %02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);

    *hours   = sTime.Hours;
    *minutes = sTime.Minutes;
    *seconds = sTime.Seconds;

}