#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)

# specify cross compilers and tools
SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
SET(CMAKE_CXX_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

PROJECT(myframe C CXX ASM)
set(CMAKE_CXX_STANDARD 11)

include_directories(lib/COMMUNICATION/I2C)
include_directories(lib/COMMUNICATION/UART)
include_directories(lib/DEVICE/BEEP)
include_directories(lib/DEVICE/KEY)
include_directories(lib/DEVICE/LED)
include_directories(lib/DEVICE/SERVO)
include_directories(lib/DEVICE/STEPPER)
include_directories(lib/DEVICE/TPAD)
include_directories(lib/SYSTEM)
include_directories(lib/SYSTEM/timer_lib)

add_executable(my_frame
        helloworld2/main.cpp
        lib/COMMUNICATION/I2C/i2c.cpp
        lib/COMMUNICATION/I2C/i2c.h
        lib/COMMUNICATION/UART/usart.cpp
        lib/COMMUNICATION/UART/usart.h
        lib/DEVICE/BEEP/beep.cpp
        lib/DEVICE/BEEP/beep.h
        lib/DEVICE/KEY/key.cpp
        lib/DEVICE/KEY/key.h
        lib/DEVICE/LED/led.cpp
        lib/DEVICE/LED/led.h
        lib/DEVICE/SERVO/servo.cpp
        lib/DEVICE/SERVO/servo.h
        lib/DEVICE/STEPPER/stepper.cpp
        lib/DEVICE/STEPPER/stepper.h
        lib/DEVICE/TPAD/tpad.cpp
        lib/DEVICE/TPAD/tpad.h
        lib/SYSTEM/timer_lib/pulse.cpp
        lib/SYSTEM/timer_lib/pulse.h
        lib/SYSTEM/timer_lib/pwm.cpp
        lib/SYSTEM/timer_lib/pwm.h
        lib/SYSTEM/timer_lib/spwm.h
        lib/SYSTEM/timer_lib/tone.h
        lib/SYSTEM/adc.cpp
        lib/SYSTEM/adc.h
        lib/SYSTEM/basic.cpp
        lib/SYSTEM/basic.h
        lib/SYSTEM/core_cm3.h
        lib/SYSTEM/debug.h
        lib/SYSTEM/dma.cpp
        lib/SYSTEM/dma.h
        lib/SYSTEM/exti.cpp
        lib/SYSTEM/exti.h
        lib/SYSTEM/stm32f10x.h
        lib/SYSTEM/sys.c
        lib/SYSTEM/sys.h
        lib/SYSTEM/system_stm32f10x.h
        lib/SYSTEM/timer.cpp
        lib/SYSTEM/timer.h
        lib/SYSTEM/wdg.cpp
        lib/SYSTEM/wdg.h)
