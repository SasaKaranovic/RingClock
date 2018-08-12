// C99
#include <stdint.h>

// HAL
#include "stm32f1xx.h"
#include "stm32f1xx_hal_conf.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx_hal_rcc.h"

// App
#include "uart.h"



/* USART2 init function */
UART_HandleTypeDef      huart1;
GPIO_InitTypeDef        GPIO_InitStruct;
HAL_StatusTypeDef       halerr;

void USART_Init(void)
{

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    __HAL_RCC_USART1_CLK_ENABLE();

   /**USART1 GPIO Configuration    
    PA9      ------> USART1_TX
    PA10     ------> USART1_RX 
    */

    GPIO_InitStruct.Pin         = GPIO_PIN_9;
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull        = GPIO_PULLUP;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin         = GPIO_PIN_10;
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Pull        = GPIO_PULLUP;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    huart1.Instance             = USART1;
    huart1.Init.WordLength      = UART_WORDLENGTH_8B;
    huart1.Init.StopBits        = UART_STOPBITS_1;
    huart1.Init.Parity          = UART_PARITY_NONE;
    huart1.Init.Mode            = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl       = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling    = UART_OVERSAMPLING_16;
    huart1.Init.BaudRate        = 115200;
    halerr = HAL_UART_Init(&huart1);
    if(halerr !=  HAL_OK) 
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        while(1);
    }




}


char u1_putchar(char c)
{
    while (!(USART1->SR & UART_FLAG_TXE));
            USART1->DR = (c & 0xFF);

    return(c);
}




void clearFlags(void)
{
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_PE);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_TXE);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_TC);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_RXNE);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_IDLE);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_LBD);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_CTS);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_ERR);

}


