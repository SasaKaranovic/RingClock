#ifndef __RTC_H__
#define __RTC_H__

void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_ReadTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

#endif