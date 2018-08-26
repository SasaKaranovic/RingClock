// C99
#include <stdint.h>

// HAL
#include "stm32f1xx.h"
#include "stm32f1xx_hal_rtc.h"
#include "stm32f1xx_hal_conf.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_tim.h"

// App
#include "clock_config.h"
#include "uart.h"
#include "debug.h"

// Local defines
#define TIM3_CCR1_Address 0x40000434  // physical memory address of Timer 3 CCR1 register

// Init typedefs
RTC_HandleTypeDef hrtc;
TIM_HandleTypeDef  htim3;
TIM_OC_InitTypeDef  sConfigOC;
GPIO_InitTypeDef GPIO_InitStructure;
DMA_HandleTypeDef hdma_tim3_ch1;

// Local functions
static void SystemClock_Config(void);
static void _Error_Handler(char * file, int line);


void Sys_Init(void)
{

    HAL_Init();
    SystemClock_Config();

    HAL_Delay(100);

    Timer3_init();

    GPIO_InitTypeDef GPIO_InitStructure;

    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    
    USART_Init();

    // Output our current MCU config
    debug("-----\t Living Room Clock \t-----");
    debug("System Clock: %ldMHz", (HAL_RCC_GetSysClockFreq()/1000000));
    debug("TIM3 CH1 Prescaler is: %d", __HAL_TIM_GET_ICPRESCALER(&htim3, TIM_CHANNEL_1));


    // GPIO setup
    // Hour button
    GPIO_InitStructure.Pin     = GPIO_HOUR_PIN;
    GPIO_InitStructure.Mode    = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull    = GPIO_PULLUP;
    GPIO_InitStructure.Speed   = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_HOUR_PORT, &GPIO_InitStructure);

    // Minute button
    GPIO_InitStructure.Pin     = GPIO_MINUTE_PIN;
    GPIO_InitStructure.Mode    = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull    = GPIO_PULLUP;
    GPIO_InitStructure.Speed   = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_MINUTE_PORT, &GPIO_InitStructure);


    // Debug/activity LED
    GPIO_InitStructure.Pin     = GPIO_DBG_LED_PIN;
    GPIO_InitStructure.Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed   = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_DBG_LED_PORT, &GPIO_InitStructure);

    // User LED
    GPIO_InitStructure.Pin     = GPIO_USR_LED_PIN;
    GPIO_InitStructure.Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed   = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_USR_LED_PORT, &GPIO_InitStructure);


    HAL_GPIO_WritePin(GPIO_DBG_LED_PORT, GPIO_DBG_LED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIO_USR_LED_PORT, GPIO_USR_LED_PIN, GPIO_PIN_RESET);

    hrtc.Instance               = RTC;
    HAL_RTC_Init(&hrtc);
    RTC_Init();

}



/** System Clock Configuration
*/
static void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL14;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}



void RTC_Init(void)
{
    HAL_StatusTypeDef status;
    RTC_TimeTypeDef  sTime;
    RTC_DateTypeDef DateToUpdate;

    // Initialize RTC
    hrtc.Instance               = RTC;
    hrtc.Init.AsynchPrediv      = RTC_AUTO_1_SECOND;
    hrtc.Init.OutPut            = RTC_OUTPUTSOURCE_NONE;
    if( (status = HAL_RTC_Init(&hrtc)) )
    {
        debug("HAL_RTC_Init ERROR: 0x%02X", status);
    }

    // Set clock to 00:00:00 if RTC is booting up for the first time
    if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2){
        sTime.Hours     = 0;
        sTime.Minutes   = 0;
        sTime.Seconds   = 0;

        if( (status = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD)) )
        {
            debug("HAL_RTC_SetTime ERROR: 0x%02X", status);
        }

        DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
        DateToUpdate.Month = RTC_MONTH_JANUARY;
        DateToUpdate.Date = 0x1;
        DateToUpdate.Year = 0x0;

        if( (status = HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD)) )
        {
            debug("HAL_RTC_SetDate ERROR: 0x%02X", status);
        }

        // Set RTC flag in RTC backup register so we know RTC
        // has been initalized and is ready to be used.
        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0x32F2);
    }
}



void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

    if(rtcHandle->Instance==RTC)
    {
        HAL_PWR_EnableBkUpAccess();
        /* Enable BKP CLK enable for backup registers */
        __HAL_RCC_BKP_CLK_ENABLE();
        /* RTC clock enable */
        __HAL_RCC_RTC_ENABLE();
    }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{
    if(rtcHandle->Instance==RTC)
    {

    /* Peripheral clock disable */
        __HAL_RCC_RTC_DISABLE();

    }
} 


/*
-----------------------DMA-------------------------
*/

void Timer3_DMA_ClearFlags(void)
{
    /* Clear all flags */
    HAL_DMA_IRQHandler(&hdma_tim3_ch1);
}

void Timer3_DMA_WaitForComplete(uint16_t *LED_BYTE_Buffer, uint16_t buffersize)
{
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*)LED_BYTE_Buffer, (uint32_t)buffersize);
    HAL_Delay(10);

    while(__HAL_TIM_GET_FLAG(&htim3, DMA_FLAG_TC6));

    HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
}

void Timer3_init(void)
{
    uint16_t PrescalerValue;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* GPIO_WS2812_PIN Configuration: TIM3 Channel 1 as alternate function push-pull */
    GPIO_InitStructure.Pin      = GPIO_WS2812_PIN;
    GPIO_InitStructure.Mode     = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed    = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_WS2812_PORT, &GPIO_InitStructure);
    
    __HAL_RCC_TIM3_CLK_ENABLE();


    /* Compute the prescaler value */
    PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
    /* Time base configuration */
    htim3.Instance = TIM3;
    htim3.Init.Period = 29; // 800kHz 
    htim3.Init.Prescaler = PrescalerValue;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&htim3);

    /* PWM1 Mode configuration: Channel1 */
    sConfigOC.OCMode = TIM_OCMODE_PWM1;;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
    
    /* configure DMA */
    /* DMA clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
    

    hdma_tim3_ch1.Instance                      = DMA1_Channel6;
    hdma_tim3_ch1.Init.Direction                = DMA_MEMORY_TO_PERIPH;     // data shifted from memory to peripheral
    hdma_tim3_ch1.Init.PeriphInc                = DMA_PINC_DISABLE;
    hdma_tim3_ch1.Init.MemInc                   = DMA_MINC_ENABLE;          // automatically increase buffer index
    hdma_tim3_ch1.Init.PeriphDataAlignment      = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch1.Init.MemDataAlignment         = DMA_MDATAALIGN_HALFWORD;
    hdma_tim3_ch1.Init.Mode                     = DMA_NORMAL;               // stop DMA feed after buffer size is reached
    hdma_tim3_ch1.Init.Priority                 = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&hdma_tim3_ch1);

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(&htim3, hdma[TIM_DMA_ID_CC1], hdma_tim3_ch1);
    __HAL_LINKDMA(&htim3, hdma[TIM_DMA_ID_TRIGGER], hdma_tim3_ch1);
    
    
    /* TIM3 CC1 DMA Request enable */
    __HAL_DMA_ENABLE(&hdma_tim3_ch1);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

/*
-----------------------DMA-------------------------
*/

static void _Error_Handler(char * file, int line)
{
    UNUSED(file);
    UNUSED(line);
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1) 
    {
    }
    /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
    UNUSED(file);
    UNUSED(line);
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */

}

#endif

