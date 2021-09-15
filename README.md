# UART-serial-monitor

###### Hand gesture control to toggle state of 4-bit counter via serial communication using STM32F407 board.


##Working
This project uses OpenCV and the MediaPipe library for hand tracking.
Gesture control is achieved by calculating the real-time distance between tip of index finger and tip of thumb of user.

The "Toggle" keyword is sent serially depending on the position of index finger and thumb.

The ARM Cortex-M4 based STM32F407 board is used to toggle the on / off state of a 4 bit counter.
The 4 external LEDs are connected to pins PD_8 to PD_11. The HAL library
provided by STMicroelectronics is used to enable UART.

UART Configurations:
1. Mode: Asynchronous
2. Baud Rate: 115200
3. Stop bits: 1
