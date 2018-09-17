#ifndef __CLOCK_H__
#define __CLOCK_H__

typedef enum clockMode_t
{
	Mode_FIRST = 0,
	ModeSameColor = 1,
	ModeRainbowStatic = 2,
	ModeRainbowDynamic = 3,
	Mode_LAST = 4
} clockMode_t;

void Clock_Clear(void);
void ClockSetColor(ws_led_t *color);
void Clock_DrawHands(void);
void Clock_Update(uint8_t ClockHour, uint8_t ClockMinute);
void Clock_DrawTime(uint8_t ClockHour, uint8_t ClockMinute);
void Clock_SendData(void);
void Clock_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void Clock_ReadTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
uint8_t Clock_GetBrightness(void);
void Clock_SetBrightness(uint8_t value);
void Clock_SetMode(clockMode_t SetMode_t);
clockMode_t Clock_GetMode(void);

#endif