# 01_HelloWorld

## Description
An input is polled to detect a high or low level.

An output is set depending on input state.

If running code on the S32K144EVB, pressing BTN0 lights up the blue LED.

![Hello World block diagram](01_HelloWorld.assets/Hello%20World%20block%20diagram.PNG)

## Design
1. Disable watchdog
2. Enable clocks to GPIO modules and configure GPIO ports:
   * GPIO input (goes to BTN0 on evaluation board)
   * GPIO output (goes to blue LED)
3. Loop:
   * If BTN0 is pressed (input = 1), Turn LED on (output = 0)
   * else (input = 0), Turn LED off (output = 1)

## Pins definitions

| Pin number | Function        |
| ---------- | --------------- |
| PTD0       | GPIO [BLUE LED] |
| PTC12      | GPIO [SW2]      |

