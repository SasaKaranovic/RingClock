TARGET=RingClock
EXECUTABLE=RingClock.elf

CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
CP=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump
SZ=arm-none-eabi-size

BIN=$(CP) -O ihex 

DEFS = -DUSE_HAL_DRIVER -DSTM32F103xB -DHSE_VALUE=8000000
STARTUP = startup_stm32f103xb.s

MCU = cortex-m3
MCFLAGS = -mcpu=$(MCU) -mthumb -mlittle-endian -mthumb-interwork

STM32_INCLUDES = -IApplication \
-IIncludes \
-IDrivers/STM32F1xx_HAL_Driver/Inc \
-IDrivers/STM32F1xx_HAL_Driver/Src \
-IDrivers/STM32F1xx_HAL_Driver/Inc/Legacy \
-IDrivers/CMSIS/Device/ST/STM32F1xx/Include \
-IDrivers/CMSIS/Include

OPTIMIZE       = -O2 -Wall -Wextra -Wunused-parameter

CFLAGS	= $(MCFLAGS)  $(OPTIMIZE)  $(DEFS) -I. -I./ $(STM32_INCLUDES)  -Wl,-T,STM32F103C8Tx_FLASH.ld
AFLAGS	= $(MCFLAGS) 

SRC =  \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc_ex.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
Application/stm32f1xx_hal_msp.c \
Application/system_stm32f1xx.c \
Application/stm32f1xx_it.c \
Application/rtc.c  \
Application/clock.c  \
Application/debug.c  \
Application/sys_init.c  \
Application/uart.c  \
Application/ws2812b.c  \
Application/main.c 

OBJDIR = .
OBJ = $(SRC:%.c=$(OBJDIR)/%.o) 
OBJ += Startup.o

all: $(TARGET).hex
	$(SZ) $(TARGET).elf

$(TARGET).hex: $(EXECUTABLE)
	$(CP) -O ihex $^ $@



$(EXECUTABLE): $(SRC) $(STARTUP)
	$(CC) $(CFLAGS) $^ -lm -lc -lnosys  -o $@

clean:
	del /s /q $(TARGET).elf
	del /s /q $(TARGET).hex


flash:
	ST-LINK_CLI.exe -c SWD UR -P "$(TARGET).hex" -V -HardRst

jlink:
	JLink -device STM32F103C8 -speed 4000 -if swd -autoconnect 1 -CommanderScript flash.jlink
