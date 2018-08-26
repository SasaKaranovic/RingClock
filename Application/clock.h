#ifndef __CLOCK_H__
#define __CLOCK_H__

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

#endif