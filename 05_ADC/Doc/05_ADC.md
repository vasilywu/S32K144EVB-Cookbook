# 05_ADC

## Description
The ADC is initialized to convert two channels using software triggers that are configured for one-shot conversions.

Each conversion requires its own software trigger.

One channel connects to a potentiometer on the EVB the other to VREFSH. The results are scaled 0 to 5000 mV.

On the evaluation board, three LEDs are used to indicate the conversion result range per the following table.

| Scaled         | LED   |
| -------------- | ----- |
| 3750 - 5000 mV | Red   |
| 2500 - 3750 mV | Green |
| 1250 - 2500 mV | Blue  |
| 0 to 1250 mV   | None  |

![ADC example block diagram](05_ADC.assets/ADC%20example%20block%20diagram.PNG)

## Design
1. Disable watchdog
2. System clocks: Initialize SOSC for 8 MHz, sysclk for 80 MHz, RUN mode for 80 MHz
3. Initialize port pins:

   * Enable clocks to Port D
     * PTD0: GPIO output - connects to blue LED
     * PTD15: GPIO output - connects to red LED
     * PTD16: GPIO output- connects to green LED
4. Initialize ADC:
   * Select SOSCDIV2_CLK for functional clock and enable it to module
   * Disable module and disable interrupt requests from module (reset default state)
   * Configure ADC for 12-bit conversions using SOSCDIV2_CLK, divided by 1
   * Configure sample time of 13 ADCK clock cycles (reset default value)
   * Select software trigger for conversions, no compare functions, no DMA and use default voltage reference pins - external pins VREFH and VREFL. (reset default value)
   * Disable continuous conversions (so there is one conversion per software trigger), disable hardware averaging, disable calibration sequence start up
5. Loop:
   * Issue ADC conversion command for the channel that is connected to the potentiometer on the NXP evaluation board. (Use ADC_SC1[0] for software triggers.)
   * Wait for conversion complete flag. When conversion is complete:
     * Read result and scale to 0 to 5000 mV (Result is in ADC_R[0] for all software triggers.)
     * Illuminate LED per voltage range
   * Issue ADC conversion command to read channel 29, ADC high reference voltage (Use ADC_SC1[0] for software triggers.)
   * Wait for conversion complete flag. When conversion is complete:
     * Read result and scale to 0 to 5000 mV (Result is in ADC_R[0] for all software triggers.)

## Pins definitions

| Pin number | Function         |
| ---------- | ---------------- |
| PTC14      | [AD12]           |
| PTD0       | GPIO [BLUE LED]  |
| PTD15      | GPIO [RED LED]   |
| PTD16      | GPIO [GREEN LED] |

