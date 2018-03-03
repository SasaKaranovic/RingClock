#include "main.h"
#include "ws2812b.h"
#include "sys_init.h"



/* Buffer that holds one complete DMA transmission
 * 
 * Ensure that this buffer is big enough to hold
 * all data bytes that need to be sent
 * 
 * The buffer size can be calculated as follows:
 * number of LEDs * 24 bytes + 42 bytes
 * 
 * This leaves us with a maximum string length of
 * (2^16 bytes per DMA stream - 42 bytes)/24 bytes per LED = 2728 LEDs
 */
uint16_t LED_BYTE_Buffer[1482];



/* This function sends data bytes out to a string of WS2812s
 * The first argument is a pointer to the first RGB triplet to be sent
 * The seconds argument is the number of LEDs in the chain
 * 
 * This will result in the RGB triplet passed by argument 1 being sent to 
 * the LED that is the furthest away from the controller (the point where
 * data is injected into the chain)
 */
void WS2812_send(ws_led_t *color, uint16_t len)
{
    Timer3_DMA_ClearFlags();

    uint8_t j;
    uint8_t led;
    uint16_t memaddr;
    uint16_t buffersize;

    buffersize = (len*24)+42;   // number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
    memaddr = 0;                // reset buffer memory index
    led = 0;                    // reset led index

    // fill transmit buffer with correct compare values to achieve
    // correct pulse widths according to color values
    while (len)
    {   

        for (j = 0; j < 8; j++)                 // GREEN data
        {
            if ( (color[led].GREEN<<j) & 0x80 ) // data sent MSB first, j = 0 is MSB j = 7 is LSB
            {
                LED_BYTE_Buffer[memaddr] = 17;  // compare value for logical 1
            }
            else
            {
                LED_BYTE_Buffer[memaddr] = 9;   // compare value for logical 0
            }
            memaddr++;
        }
        
        for (j = 0; j < 8; j++)                 // RED data
        {
            if ( (color[led].RED<<j) & 0x80 )   // data sent MSB first, j = 0 is MSB j = 7 is LSB
            {
                LED_BYTE_Buffer[memaddr] = 17;  // compare value for logical 1
            }
            else
            {
                LED_BYTE_Buffer[memaddr] = 9;   // compare value for logical 0
            }
            memaddr++;
        }
        
        for (j = 0; j < 8; j++)                 // BLUE data
        {
            if ( (color[led].BLUE<<j) & 0x80 )  // data sent MSB first, j = 0 is MSB j = 7 is LSB
            {
                LED_BYTE_Buffer[memaddr] = 17;  // compare value for logical 1
            }
            else
            {
                LED_BYTE_Buffer[memaddr] = 9;   // compare value for logical 0
            }
            memaddr++;
        }
        
        led++;
        len--;
    }
    
    // add needed delay at end of byte cycle, pulsewidth = 0
    while(memaddr < buffersize)
    {
        LED_BYTE_Buffer[memaddr] = 0;
        memaddr++;
    }

    Timer3_DMA_WaitForComplete(LED_BYTE_Buffer, buffersize);
}