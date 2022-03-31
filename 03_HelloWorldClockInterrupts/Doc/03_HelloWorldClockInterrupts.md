# 03_HelloWorldClockInterrupts

## Description
Instead of software polling the timer flag, the interrupt handler clears the flag and toggles the output.

The timeout is again one second using SPLL_DIV2_CLK for the LPIT0 timer clock.

To initialize an interrupt three writes to the NVIC are taken in this project:

* Clear any prior pending interrupt (in case there was one)
  * Write a 1 to the interrupt # bit in Interrupt Clear Pending Register (ICPR)
* Enable the desired interrupt #
  * Write a 1 to the interrupt # bit in the Interrupt Set Enable Register (ISER)
* Set the interrupt’s priority
  * Write a priority from 0 to 15 to the appropriate Interrupt Priority register (IP)

![Hello World + Interrupts block diagram](03_HelloWorldClockInterrupts.assets/Hello%20World%20+%20Interrupts%20block%20diagram.PNG)

## Design
1. Disable watchdog
2. Initialize port pins:
   * Enable clock to Port D module
   * PTD0: GPIO output (goes to blue LED)
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
     * Enable channel’s interrupt
     * Timeout = 1 second of clocks
     * Set Mode to 32 bit counter and enable channel 0
6. Change Normal RUN mode clock to SPLL
   * Initialize clock dividers for CORE, BUS and FLASH for new target clock frequency
   * Switch system clock input to SPLL (160 MHz before dividers)
7. Loop: wait forever
8. LPIT_0 Channel 0 Interrupt Handler:
   * Clear channel flag
   * Increment counter
   * Toggle GPIO output

## Pins definitions

| Pin number | Function        |
| ---------- | --------------- |
| PTD0       | GPIO [BLUE LED] |

## ARM: Cortex-M3/M4 Interrupts Happening Twice?

### CAUSE
This may happen with devices:

* That add an external, system-level write buffer in their Cortex-M3 or Cortex-M4 design, AND
* The ISR code exits immediately after a write to clear the interrupt.

In this situation, the write to clear the interrupt may not complete before interrupt triggers a second time.

### REASON

For the Cortex-M3 and Cortex-M4 cores, writes to memory (STR, STMIA or PUSH) are internally buffered. The Harvard architecture allows the MCU to fetch and execute instructions without waiting for data writes to memory to complete. The Cortex-M cores are aware of the internal buffer and prevent subsequent interrupts until the internal buffer empties and the write completes.

Sometimes vendors incorporate an additional external, system-level write buffer in their Cortex-M3 and Cortex-M4 designs for better performance. But unfortunately, the core is not aware of this external write buffer and cannot access it's status. For these externally-buffered devices, if an ISR exits immediately after clearing the interrupt register, a second interrupt could trigger again before the write to clear the interrupt completes.

For example, this ISR exits immediately after clearing the timer interrupt. Without the external buffer implementation, this code would work as expected. However, on a device with an external, system-level write buffer, this code could cause this "double IRQ" condition:

```c
void Timer_IRQHandler (void) {
  Timeout_counter++;                /* Increment timeout counter */
  PortD->PTOR |= 1<<0;              /* Toggle output on port D0  */
  Timer->MSR |= TIMER_MASK;         /* Clear timer interrupt     */
}
```

### RESOLUTION

Using the DSB instruction or __dsb(0) intrinsic before exiting will force a wait for the internal write buffer to empty, but that instruction cannot test the status of an optional system-level write buffer if there happens to be one. To make sure the peripheral interrupt register gets set properly, just perform another memory write before exiting the ISR.

Given the example above, one way to do this is by incrementing the timeout counter AFTER clearing the interrupt:

```c
void Timer_IRQHandler (void) {
  PortD->PTOR |= 1<<0;              /* Toggle output on port D0         */
  Timer->MSR |= TIMER_MASK;         /* Clear timer interrupt            */
  Timeout_counter++;                /* Count timeout & insure IRQ clear */
}
```

Any type of memory write will accomplish this. The Timeout_counter\+\+ works because it performs a read-modify-write to memory. If you can't move an instruction like in the above example, just add harmless code that performs a read-write like this:

```c
void Timer_IRQHandler (void) {
  Timeout_counter++;                /* Increment timeout counter */
  PortD->PTOR |= 1<<0;              /* Toggle output on port D0  */
  Timer->MSR |= TIMER_MASK;         /* Clear timer interrupt     */
  PortD->PTOR = PortD->PTOR;        /* Insure IRQ clear          */
}
```

