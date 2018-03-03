// C99
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// HAL
#include "stm32f1xx.h"
#include "stm32f1xx_hal_conf.h"
#include "stm32f1xx_hal_rcc.h"

//Static functions
static void vprint(const char *fmt, va_list argp);

void debugPrint(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
}


static void vprint(const char *fmt, va_list argp)
{
	_UNUSED(debugPrint);

    char string[200];
    char *i = &string[0];
    if(0 < vsprintf(string,fmt,argp)) // build string
    {
	    while (*i)
	    { 
	        u1_putchar(*i++);
	    }
    }
}
