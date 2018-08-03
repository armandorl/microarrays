/* README */
/* Project by Armando Ruiz */

/* Description */
This project was developed using Microchip dsPic33EP256GP502. 
It receives the input of 4 microphones converts them using the ADC and stores the values using the DMA.

The code is driven by the interruption of the DMA, which happens after the configured buffer is filled.
Currently only performs calculations on the frequency and phase of the first 2 input signals, due to limitations of the dsp Library, it is necessary to transform the signal into a complex signal which makes the processing slower.
2 Buffers are used in order to alternate the use of the buffers, while one buffer gets filled, the other buffer is used for calculations.

/* Hardware */
The hardware consists of 4 Electrostatic Microphones with amplifiers as inputs for the dsPic ADC's.
The dsPic does not have any additional connections, however pins RB14 and RB15 are used as follows:
RB14 - Toggles on every DMA interrupt
RB15 - Toggles on processing of ADC signals.

/* Configuration and calculations */
See the PIC_Calculations.xlsx file for the following:
Configuration of PIC: CPU Clocking system and ADCs.
Execution Time analysis - contains information on how much it takes to execute each function.
InputSignalSim - contains examples of simulated signals that can be input on the MPLAB simulator.

/* Input signal */
analogueInputSim.txt - File with 4 signals that can be used in the MPLAB simulator.

/* Compiler */
The compiler used is the Microchip XC16 compiler. Once the Microchip bin folder is set in the PATH, just run the make command in the root of this project.






