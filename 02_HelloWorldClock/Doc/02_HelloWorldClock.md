# 02_HelloWorldClock

## Description
Core clock is set to 80 MHz. LPIT0 channel 0 is configured to count one second of SPLL clocks.

Software polls the channels timeout flag and toggles the GPIO output to the LED when the flag sets.

![Hello World + Clocks block diagram](02_HelloWorldClock.assets/Hello%20World%20+%20Clocks%20block%20diagram.PNG)

## Design
1. Disable watchdog
2. Enable clocks to GPIO modules and configure GPIO ports:
   * GPIO output (goes to blue LED)
3. Initialize system oscillator (SOSC) for 8 MHz crystal
   * Initialize desired SOSC dividers
   * Configure range, high gain, reference
   * Ensure SOSC Control and Status register is unlocked
   * Enable SOSC in SOSC Control and Status register
   * Wait for SOSC to be valid
4. Initialize System PLL (SPLL) to 160 MHz using 8 MHz SOSC
   * Ensure SPLL is disable to allow configuration
   * Initialized desired SPLL dividers
   * Initialize PLL Ref Clk Divider and Sys PLL Multiplier 1
   * Fpll = Fosc / PLL Ref Clk Divider x Sys PLL Multiplier / 2 = 8 MHz / 1 x 20 / 2= 160 MHz
   * Ensure SPLL Control and Status register is unlocked
   * Enable SPLL in SPLL Control and Status register
   * Wait for SPLL to be valid
5. Initialize LPIT0 channel 0:
   * Enable clock source of SPLL_DIV2_CLK
   * Enable clock to LPIT0 registers
   * Enable LPIT0 module
   * Initialize channel 0:
     * Timeout = 1 second of clocks
     * Set Mode to 32 bit counter and enable channel 0
6. Change Normal RUN mode clock to SPLL
   * Initialize clock dividers for CORE, BUS and FLASH for new target clock frequency
   * Switch system clock input to SPLL (160 MHz before dividers)
7. Loop:
   * Wait for LPIT0 channel 0 flag
   * Increment counter, toggle GPIO output and clear channel flag

## Pins definitions

| Pin number | Function        |
| ---------- | --------------- |
| PTD0       | GPIO [BLUE LED] |

