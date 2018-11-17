# My New RingClock

I wanted to build myself a cool clock that would look nice, be different and make absolutely no sound. And so I did. 

Check out [this post to see how I did it](https://sasakaranovic.com/projects/ringclock/)

# Requirements

In order to compile this firmware, you will need:
- [ARM GCC compiler](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads) installed and added to your system environment variables (if you are using windows)
- Programer to flash the compiled firmware onto the board (ST-Link or JLink)

# Compiling the firmware
Open your command prompt or terminal and navigate to Firmware folder (this one).
Start compiling the firmware by typing `make all`
After it's complete you should see something like

```
arm-none-eabi-size RingClock.elf
   text    data     bss     dec     hex filename
  39976    1108    5096   46180    b464 RingClock.elf
```

This should mean that you successfully compiled the firmware.

# Flashing the firmware onto the board
Depending which programmer you are using, you can either use provided GUI app
to flash the firmware (STM32 ST-LINK Utility or J-Flash). I personally prefer  doing
this through console so I've added two options to the make file that allow you to flash
firmware onto the board by calling  `make flash` for ST-Link  or `make jlink` for J-Link programmer.