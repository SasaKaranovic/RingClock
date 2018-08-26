#ifndef __CLOCK_CONFIG_H__
#define __CLOCK_CONFIG_H__

// Clock related hard-coded values
#define LED_COUNT               60      // Number of LEDs we have 
#define LED_OFFSET              30      // Position of 12HR LED, counting CCW from your first DIN LED
#define DIMMING_FEATURE_ENABLED         // Comment this line to disable LED time-based dimming
#define DIM_HOUR_TURN_ON        19      // Turn ON dim feature at 19:00
#define DIM_HOUR_TURN_OFF       8       // Turn OFF dim feature at 08:00
#define DIM_BRIGHTNESS_ON       200     // Dim to 30% of full intensity
#define DIM_BRIGHTNESS_OFF      10      // Dim to 80% of full intensity
    
// GPIO definitions 
#define GPIO_MINUTE_PIN         GPIO_PIN_3
#define GPIO_MINUTE_PORT        GPIOB
    
#define GPIO_HOUR_PIN           GPIO_PIN_15
#define GPIO_HOUR_PORT          GPIOA
    
#define GPIO_WS2812_PIN         GPIO_PIN_6
#define GPIO_WS2812_PORT        GPIOA
    
#define GPIO_DBG_LED_PIN        GPIO_PIN_6
#define GPIO_DBG_LED_PORT       GPIOB
    
#define GPIO_USR_LED_PIN        GPIO_PIN_7
#define GPIO_USR_LED_PORT       GPIOB


// Convenience macros and definitions
#define BUTTON_ReadHR()     (!HAL_GPIO_ReadPin(GPIO_HOUR_PORT,   GPIO_HOUR_PIN))
#define BUTTON_ReadMIN()    (!HAL_GPIO_ReadPin(GPIO_MINUTE_PORT, GPIO_MINUTE_PIN))

#ifdef DEBUG_BUILD

#define DebugLED_On()       (HAL_GPIO_WritePin(GPIO_DBG_LED_PORT, GPIO_DBG_LED_PIN, GPIO_PIN_SET))
#define DebugLED_Off()      (HAL_GPIO_WritePin(GPIO_DBG_LED_PORT, GPIO_DBG_LED_PIN, GPIO_PIN_RESET))
#define DebugLED_Toggle()   (HAL_GPIO_TogglePin(GPIO_DBG_LED_PORT, GPIO_DBG_LED_PIN))

#else

#define DebugLED_On()   
#define DebugLED_Off()  
#define DebugLED_Toggle()

#endif

#define UserLED_On()        (HAL_GPIO_WritePin(GPIO_USR_LED_PORT, GPIO_USR_LED_PIN, GPIO_PIN_SET))
#define UserLED_Off()       (HAL_GPIO_WritePin(GPIO_USR_LED_PORT, GPIO_USR_LED_PIN, GPIO_PIN_RESET))
#define UserLED_Toggle()    (HAL_GPIO_TogglePin(GPIO_USR_LED_PORT, GPIO_USR_LED_PIN))

#define UNUSED(x) ((void)(x))

#endif