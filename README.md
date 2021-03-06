# S32K144EVB-Cookbook
The practices of the S32K14x Series Cookbook Application Note.

They will be implemented using Keil uVision5 (MDK-Community Edition) and tested on the S32K144 evaluation board (S32K144EVB-Q100). 

## S32K144EVB-Q100

The S32K144EVB is a low-cost evaluation and development board for general purpose industrial and automotive applications.

![S32K144EVB_GS-PINS](README.assets/S32K144EVB_GS-PINS.png)

Based on the 32-bit Arm® Cortex®-M4F S32K14 MCU, the S32K144EVB offers a standard-based form factor compatible with the Arduino® UNO pin layout, providing a broad range of expansion board options for quick application prototyping and demonstration.

## MDK Community Edition

MDK-Community is the full-featured, non-commercial software development suite for Cortex®-M-based microcontrollers.

![Compare MDK Editions](README.assets/Compare%20MDK%20Editions.PNG)

## P&E OpenSDA
OpenSDA is an open-standard serial and debug adapter. It bridges serial and debug communications between a USB host and an
embedded target processor. 

NXP OpenSDA has a P&E and a CMSIS-DAP mode depending on the firmware loaded into the OpenSDA processor U8. We do not need an external debugger such as a ULINK2. 

1. Click on Settings: The P&E Connection Manager window opens.

   ![P&E](README.assets/P&E.png)

2. In the Interface box, select OpenSDA Embedded Tower Debug - USB Port: 

   ![OpenSDA Embedded Tower Debug](README.assets/OpenSDA%20Embedded%20Tower%20Debug.png)

3. Click the Select New Device box, and select your exact processor: 

   ![Select New Device box](README.assets/Select%20New%20Device%20box.png)

4. Click on the Refresh List and get a valid Port:

   ![Refresh List](README.assets/Refresh%20List.png)

## Create New Keil Project

1. Project->New uVision Project

2. Select Device: S32K144HAxxxLLx

   ![Select Device](README.assets/Select%20Device.png)

3. Manage Project Items

   ![Manage Project Items](README.assets/Manage%20Project%20Items.png)

4. Manage RTE: Ensure the CORE and Startup are checked

   ![Manage RTE](README.assets/Manage%20RTE.png)

5. Add Include Paths

   ![Add Include Path](README.assets/Add%20Include%20Path.png)

6. Change the Optimization Level to "-O0", it's important for Single-Stepping

   ![Change Optimization](README.assets/Change%20Optimization.png)

7. Uncheck "Use Memory Layout from Target Dialog"  and Select "S32K144_64_flash.sct" as Scatter File

   ![Scatter](README.assets/Scatter.png)

## Lists

| Number | Project                      | Description                                                  |
| ------ | ---------------------------- | ------------------------------------------------------------ |
| 01     | Hello World                  | Simplest project:<br/>Configure GPIO<br/>Output to LED follows switch input |
| 02     | Hello World Clock            | Perform common initialization for clocks and LPIT:<br/>Initialize System Oscillator (SOSC)<br/>Initialize LPIT channel (1 second timeout)<br/>Toggle output to LED every LPIT timeout |
| 03     | Hello World Clock Interrupts | The Hello World + Clock example is modified to service the PIT channel timeout with an interrupt service route:<br/>Initialize System Oscillator (SOSC)<br/>Initialize an LPIT channel for 1 second timeout and enable its interrupt<br/>Wait forever<br/>At LPIT timeout interrupt, toggle output to LED |
| 04     | Comparator                   | Compare an analogic input with a software configurable reference.<br/>LED will turn red if the input voltage does not overcome the given threshold<br/>and green if it does |
| 05     | ADC                          | Perform simple analog to digital conversions using software trigger:<br/>Initialize ADC for SW trigger, continuous mode<br/>loop:<br/>Convert channel connected to pot on evaluation board<br/>Scale result to 0 to 5000 mV<br/>Light evaluation board LEDs to reflect voltage level<br/>Convert channel connected to the ADC high reference voltage |
| 06     | CAN                          | Transmit and receive an eight byte CAN 2.0 message at 500 KHz:<br/>Initialize FlexCAN and Message Buffer 4 to receive a message<br/>Transmit one frame using Message Buffer 0<br/>Loop:<br/>If Message Buffer 4 received message flag is set, read message<br/>If Message Buffer 0 transmit done flag is set, transmit another message |
| 07     | CAN FD                       | Transmit and receive a 64 byte CAN FD message at 500 KHz and 1 or 2 MHz:<br/>Initialize FlexCAN and Message Buffer 4 to receive a message<br/>Transmit one frame using Message Buffer 0<br/>Loop:<br/>If Message Buffer 4 received message flag is set, read message<br/>If Message Buffer 0 transmit done flag is set, transmit another message |
| 08     | LPSPI                        | Transmit and receive a SPI frame:<br/>Initialize LPSPI for 1M Baud, PCS3 which is connected to SPC on EVB<br/>Wait for Tx FIFO to have at least one available slot then issue transmit<br/>Wait for Rx FIFO to have at least one received frame then read data |

