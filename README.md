# STM32 Stopwatch with Photosensor

# Stopwatch starting and stopping by a photosensor.
# 7-segmented display.
# Usage of I2C and interrupts.

# Exact device: EVALKIT-VL6180X
# Proximity and ambient light sensing (ALS) module.

# Development Environment Components:
# GCC: Use arm-eabi-gcc for compiling.
# Binutils: Handy tools include arm-eabi-ar, arm-eabi-as, etc.
# GDB: Debugger for ARM with arm-eabi-gdb.
# Newlib: Standard C library for embedded systems.
# CMSIS: Header files provided by ARM for Cortex microcontrollers.
# OpenOCD: On-Chip Debugger, use openocd for programmer support.
# Serial communication tool for /dev/tty*, e.g., minicom.

# After installing the necessary tools, set symbolic links and add them to the path in .bashrc, e.g.

# PATH=$PATH:/home/user/arm/bin
# export PATH

# Usage: /home/user/arm/stm32/ocd/qfn4
# Loads code into a microcontroller.

# Device usage:
# Move your hand slowly at a distance of about 15 cm to start the countdown; move again to stop the stopwatch.



