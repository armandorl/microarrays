/* 
 * File:   vad.c
 * Author: arm-asus
 *
 * Created on 22 de septiembre de 2013, 01:09 PM
 */
/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#elif defined(__C30__)
    #if defined(__dsPIC33E__)
    	#include <p33Exxxx.h>
    #elif defined(__dsPIC33F__)
    	#include <p33Fxxxx.h>
    #endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dsp.h>

#include "fft.h"
#include "user.h"
#include "serialDriver.h"

#define CALIBRATION_START 100  // Value * 100ms cycle
#define CALIBRATION_END   1000

UINT8 StartFlag = 0;

/*
 * 
 */

void storeValues(void)
{
    static UINT16 counter = 0;
    INTCON2bits.GIE = 0; // Disable global interrupts
    PORTBbits.RB14 ^= 1;
    
    if(activeBuffer == 0)
    {
        BufferA_regs[0][counter] = ADC1BUF0;
        BufferA_regs[1][counter] = ADC1BUF1;
       
    }
    else
    {
        BufferB_regs[0][counter] = ADC1BUF0;
        BufferB_regs[1][counter] = ADC1BUF1;
        
    }
    
    counter++;
    if(counter == BLOCKSIZE)
    {
        activeBuffer ^= 1;
        activeBuffer &= 0x1;
        startService = 1;

    }

    counter = counter % BLOCKSIZE;

    INTCON2bits.GIE = 1; // Enable global interrupts
    IFS0bits.AD1IF = 0; // ADC Interrupt flag
}

INT16 ProcessADCSamples(INT16 *signal, UINT8 channel)
{

    /* Remove the signal offset */
    
    

    /* Detect if voice is available */

    return 0;

}

void CalculateAverage(INT16 *signal, UINT8 channel)
{

        // Calibration only required once
//    CHANNEL_OFFSET[channel] = ( CHANNEL_OFFSET[channel] +
//                                Calibrate(signal) ) / 2;
      


}


INT8 calibration(void)
{
    static UINT16 initCounter = 0;
    INT16 InProgress = 1;
    
    if (initCounter > CALIBRATION_START && initCounter <= CALIBRATION_END)
    {
        if (activeBuffer == 0)
        {
            CalculateAverage((INT16*)&BufferB_regs[0][0], 0);
            CalculateAverage((INT16*)&BufferB_regs[1][0], 1);

        }
        else
        {
            CalculateAverage((INT16*)&BufferA_regs[0][0], 0);
            CalculateAverage((INT16*)&BufferA_regs[1][0], 1);

        }
        writeString(".");
        initCounter++;
    }
    else if (StartFlag == 0 && initCounter > CALIBRATION_END)
    {
        StartFlag = 1;
        writeString("\n\rMicrophone 1 offset:\n\r");
        writeString("\n\rMicrophone 2 offset:\n\r");
        InProgress = 0;

    }
    else if (StartFlag == 1)
    {
        // Skip operation
    }
    else
    {
        initCounter++;
        if (initCounter == CALIBRATION_START )
        {

            writeString("Calibration started");
        }
    }

    return InProgress;

}
void adcService(void)
{
    
    
    INT16 peakFrequencyBin0 = 0;           /* Declare post-FFT variables to compute the */
    INT16 peakFrequencyBin1 = 0;           /* Declare post-FFT variables to compute the */
    INT32 peakFrequency = 0;    /* frequency of the largest spectral component */
    INT16 squaredOutput[BLOCKSIZE] = {0};
    PORTBbits.RB15 ^= 1;
//    writeString("x");
    
    if(StartFlag == 1)
    {
        if (activeBuffer == 0)
        {
            writeString("\033[2J"); // Clear terminal
            // Register B has just been written so it can be processed
//            writeString("0:");
//            ScaleSignal(&BufferB_regs[0][0]);
//            writeString("1:");
//            ScaleSignal(&BufferB_regs[1][0]);
            writeString("A:");
            FFTReal32bIP(LOG2_BLOCK_LENGTH, FFT_BLOCK_LENGTH, &BufferB_regs[0][0],
                         (int *) __builtin_psvoffset(&twiddleFactors[0]),
                         (int) __builtin_psvpage(&twiddleFactors[0]));
            writeString("B:");
            FFTReal32bIP(LOG2_BLOCK_LENGTH, FFT_BLOCK_LENGTH, &BufferB_regs[0][0],
                         (int *) __builtin_psvoffset(&twiddleFactors[0]),
                         (int) __builtin_psvpage(&twiddleFactors[0]));
            writeString("C:");
            
            SquareMagnitude(FFT_BLOCK_LENGTH, &BufferB_regs[0][0], &squaredOutput[0]);
            writeString("D1:");
            SquareMagnitude(FFT_BLOCK_LENGTH, &BufferB_regs[1][0], &squaredOutput[0]);
            writeString("D2:");
            /* Find the frequency Bin ( = index into the SigCmpx[] array) that has the largest energy*/
            /* i.e., the largest spectral component */
            VectorMax(FFT_BLOCK_LENGTH/2, &BufferB_regs[0][0], &peakFrequencyBin0);
            VectorMax(FFT_BLOCK_LENGTH/2, &BufferB_regs[1][0], &peakFrequencyBin1);

            writeString("E:");
            /* Compute the frequency (in Hz) of the largest spectral component */
            peakFrequency = peakFrequencyBin0*(SAMPLING_RATE/FFT_BLOCK_LENGTH);
            writeNumber(peakFrequency);
            writeString(":");
            peakFrequency = peakFrequencyBin1*(SAMPLING_RATE/FFT_BLOCK_LENGTH);
            writeNumber(peakFrequency);
            writeString("F:");
            writeString("\n\r");
            

        }
        else
        {
//            writeString("\033[2J"); // Clear terminal
//            writeString("0:");
//            ScaleSignal(&BufferA_regs[0][0]);
//            writeString("1:");
//            ScaleSignal(&BufferA_regs[1][0]);
            writeString("G:");
            FFTReal32bIP(LOG2_BLOCK_LENGTH, FFT_BLOCK_LENGTH, &BufferA_regs[0][0],
                         (int *) __builtin_psvoffset(&twiddleFactors[0]),
                         (int) __builtin_psvpage(&twiddleFactors[0]));
            writeString("H:");
            FFTReal32bIP(LOG2_BLOCK_LENGTH, FFT_BLOCK_LENGTH, &BufferA_regs[0][0],
                         (int *) __builtin_psvoffset(&twiddleFactors[0]),
                         (int) __builtin_psvpage(&twiddleFactors[0]));

            writeString("I:");

            SquareMagnitude(FFT_BLOCK_LENGTH, &BufferA_regs[0][0], &squaredOutput[0]);
            writeString("J1:");
            SquareMagnitude(FFT_BLOCK_LENGTH, &BufferA_regs[1][0], &squaredOutput[0]);
            writeString("J2:");
            /* Find the frequency Bin ( = index into the SigCmpx[] array) that has the largest energy*/
            /* i.e., the largest spectral component */
            VectorMax(FFT_BLOCK_LENGTH/2, &BufferA_regs[0][0], &peakFrequencyBin0);
            VectorMax(FFT_BLOCK_LENGTH/2, &BufferA_regs[1][0], &peakFrequencyBin1);

            writeString("K:");
            /* Compute the frequency (in Hz) of the largest spectral component */
            peakFrequency = peakFrequencyBin0*(SAMPLING_RATE/FFT_BLOCK_LENGTH);
            writeNumber(peakFrequency);
            writeString(":");
            peakFrequency = peakFrequencyBin1*(SAMPLING_RATE/FFT_BLOCK_LENGTH);
            writeNumber(peakFrequency);
            writeString("L:");
            writeString("\n\r");
            
        }
    }
    
}
