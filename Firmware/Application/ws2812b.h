#ifndef __WS2812B_H__
#define __WS2812B_H__

typedef struct ws_led_t {
   uint8_t RED;
   uint8_t GREEN;
   uint8_t BLUE;
} ws_led_t;

void Timer3_init(void);
void WS2812_send(ws_led_t *color, uint16_t len);

#endif