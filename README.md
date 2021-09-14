# UART-serial-monitor
Serial communication is established with STM32F407 board via UART to control and monitor counter with 7-segment display.

This project will start a binary 4-bit counter using external LEDs connected on GPIO pins PD_8 - PD_11.
Serial communication is established via UART to monitor the count status. Counter start / stop state is toggled if keyword "Toggle" is received via UART.
