#ifndef __SYS_INIT_H__
#define __SYS_INIT_H__


void Sys_Init(void);
void RTC_Init(void);

void Timer3_DMA_ClearFlags(void);
void Timer3_DMA_WaitForComplete(uint16_t *LED_BYTE_Buffer, uint16_t buffersize);

void assert_failed(uint8_t* file, uint32_t line);

#define BUTTON_ReadHR()		(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3))
#define BUTTON_ReadMIN()	(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2))


#endif