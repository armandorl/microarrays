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

#define HALF_ADC_OFFSET       512

UINT8 StartFlag = 0;

/*
 * 
 */

void storeValues(void)
{
    static UINT16 counter = 0;
    static FLOAT32 FrameEnergy = 0;
    FLOAT32 tempVal = 0;
    INTCON2bits.GIE = 0; // Disable global interrupts
    
    
    if(activeBuffer == 0)
    {
        BufferA0_regs[counter].real = ADC1BUF0;
        BufferA1_regs[counter].real = ADC1BUF1;
        BufferA2_regs[counter].real = ADC1BUF2;
        BufferA3_regs[counter].real = ADC1BUF3;

        BufferA0_regs[counter].imag = 0;
        BufferA1_regs[counter].imag = 0;
        BufferA2_regs[counter].imag = 0;
        BufferA3_regs[counter].imag = 0;

        tempVal = (((float)BufferA0_regs[counter].real / 1024) * 2) - 1;
        if(tempVal < 0) tempVal = tempVal * -1;
        FrameEnergy += tempVal;
        tempVal = (((float)BufferA1_regs[counter].real / 1024) * 2) - 1;
        if(tempVal < 0) tempVal = tempVal * -1;
        FrameEnergy += tempVal;
        tempVal = (((float)BufferA2_regs[counter].real / 1024) * 2) - 1;
        if(tempVal < 0) tempVal = tempVal * -1;
        FrameEnergy += tempVal;
        tempVal = (((float)BufferA3_regs[counter].real / 1024) * 2) - 1;
        if(tempVal < 0) tempVal = tempVal * -1;
        FrameEnergy += tempVal;

        
    }
    else
    {
        BufferB0_regs[counter].real = ADC1BUF0;
        BufferB1_regs[counter].real = ADC1BUF1;
        BufferB2_regs[counter].real = ADC1BUF2;
        BufferB3_regs[counter].real = ADC1BUF3;

        BufferB0_regs[counter].imag = 0;
        BufferB1_regs[counter].imag = 0;
        BufferB2_regs[counter].imag = 0;
        BufferB3_regs[counter].imag = 0;

        tempVal = (((float)BufferB0_regs[counter].real / 1024) * 2) - 1;
        if(tempVal < 0) tempVal = tempVal * -1;
        FrameEnergy += tempVal;
        tempVal = (((float)BufferB1_regs[counter].real / 1024) * 2) - 1;
        if(tempVal < 0) tempVal = tempVal * -1;
        FrameEnergy += tempVal;
        tempVal = (((float)BufferB2_regs[counter].real / 1024) * 2) - 1;
        if(tempVal < 0) tempVal = tempVal * -1;
        FrameEnergy += tempVal;
        tempVal = (((float)BufferB3_regs[counter].real / 1024) * 2) - 1;
        if(tempVal < 0) tempVal = tempVal * -1;
        FrameEnergy += tempVal;
        
    }
    
    counter++;
    if(counter == FFT_BLOCK_LENGTH)
    {
        activeBuffer ^= 1;
        activeBuffer &= 0x1;
//        writeString("Energy for Frame:");
//        writeRoundFloatNum(FrameEnergy);
//        writeString("\n\r");
        if(FrameEnergy > 60)
        {
            startService = 1;
            StartFlag = 0;
        }
        else
        {
            // Do Calibration only in frames with no voice
            StartFlag = 1;
            startService = 0;
        }

        FrameEnergy = 0;

    }

    counter = counter % FFT_BLOCK_LENGTH;

    INTCON2bits.GIE = 1; // Enable global interrupts
    IFS0bits.AD1IF = 0; // ADC Interrupt flag
    PORTBbits.RB14 ^= 1;
}

INT16 ProcessADCSamples(INT16 *signal, UINT8 channel)
{

    /* Remove the signal offset */
    
    

    /* Detect if voice is available */

    return 0;

}

FLOAT32 CalculateAverage(fractcomplex *signal)
{

    FLOAT32 tempVal = 0;
    INT32 i = 0;
    
    for(i=0; i < FFT_BLOCK_LENGTH; i++)
    {
        tempVal += ((((float)signal[i].real / 1024) * 200) - 100);
    }
    tempVal /= FFT_BLOCK_LENGTH;

    return tempVal;


}


void calibration(void)
{
 
    static UINT8 CalFinishFlag = 0;
    static FLOAT32 tempVal;

    writeString("Calibration menu:\n\r");
    writeString("1. Press 's' to start calibration\n\r");
    writeString("2. Press 'k' to skip calibration\n\r");
    INT8 keypressed = getCharacter();
    while ( keypressed != 's' && keypressed != 'k')
    {
        keypressed = getCharacter();
    }

    if( keypressed == 's')
    {
        do
        {

            writeString("\033[2J"); // Clear terminal
            writeString("\033[H"); // Move cursor to upper left corner
            writeString("Adjust the pots until the value of the microphones is 0+/-10\r\n");
            writeString("Press 'e' when you finish the calibration\r\n");
            writeString("Press 'c' to refresh the microphone values\r\n");
            
            if(StartFlag)
            {
                if (activeBuffer == 0)
                {
                    tempVal = CalculateAverage(&BufferB0_regs[0]);
                    writeString("Mic 1:");
                    writeRoundFloatNum(tempVal);
                    writeString("\r\n");
                    tempVal = CalculateAverage(&BufferB1_regs[0]);
                    writeString("Mic 2:");
                    writeRoundFloatNum(tempVal);
                    writeString("\r\n");
                    tempVal = CalculateAverage(&BufferB2_regs[0]);
                    writeString("Mic 3:");
                    writeRoundFloatNum(tempVal);
                    writeString("\r\n");
                    tempVal = CalculateAverage(&BufferB3_regs[0]);
                    writeString("Mic 4:");
                    writeRoundFloatNum(tempVal);
                    writeString("\r\n");

                }
                else
                {
                    tempVal = CalculateAverage(&BufferA0_regs[0]);
                    writeString("Mic 1:");
                    writeRoundFloatNum(tempVal);
                    writeString("\r\n");
                    tempVal = CalculateAverage(&BufferA1_regs[0]);
                    writeString("Mic 2:");
                    writeRoundFloatNum(tempVal);
                    writeString("\r\n");
                    tempVal = CalculateAverage(&BufferA2_regs[0]);
                    writeString("Mic 3:");
                    writeRoundFloatNum(tempVal);
                    writeString("\r\n");
                    tempVal = CalculateAverage(&BufferA3_regs[0]);
                    writeString("Mic 4:");
                    writeRoundFloatNum(tempVal);
                    writeString("\r\n");

                }
                
            }
            while ( keypressed != 'e' && keypressed != 'c')
            {   
                keypressed = getCharacter();
            }
            if (keypressed == 'e')
            {
                CalFinishFlag = 1;
            }
            keypressed = 0;
        }
        while(!CalFinishFlag);
    }

}

/* Variables for adcService */
INT16 peakFrequencyBin0 = 0;           /* Declare post-FFT variables to compute the */
INT16 peakFrequencyBin1 = 0;           /* Declare post-FFT variables to compute the */

void adcService(void)
{
    
//    INT32 peakFrequency = 0;    /* frequency of the largest spectral component */
//    INT16 squaredOutput[FFT_BLOCK_LENGTH / 2] = {0};
    PORTBbits.RB15 ^= 1;
//    writeString("x");
    
    if(StartFlag == 1)
    {
        if (activeBuffer == 0)
        {
           // writeString("\033[2J"); // Clear terminal
            // Register B has just been written so it can be processed
            //ScaleSignal(&BufferB0_regs[0]);
            
//            writeString("1:");
////            ScaleSignal(&BufferB1_regs[0]);
//            writeString("A:");
//            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferB0_regs[0],
//                         &twiddleFactors[0], COEFFS_IN_DATA);
//            writeString("B:");
//            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferB1_regs[0],
//                         &twiddleFactors[0], COEFFS_IN_DATA);
//            writeString("C1:");
//            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &BufferB0_regs[0], &squaredOutput[0]);
//            writeString("C2:");
//            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin0);
//            writeString("D1:");
//            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &BufferB1_regs[0], &squaredOutput[0]);
//            writeString("D2:");
            /* Find the frequency Bin ( = index into the SigCmpx[] array) that has the largest energy*/
            /* i.e., the largest spectral component */
//            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin1);
//
//            writeString("E:");
//            /* Compute the frequency (in Hz) of the largest spectral component */
//            peakFrequency = peakFrequencyBin0*(SAMPLING_RATE/FFT_BLOCK_LENGTH);
//            writeNumber(peakFrequency);
//            writeString(":");
//            peakFrequency = peakFrequencyBin1*(SAMPLING_RATE/FFT_BLOCK_LENGTH);
//            writeNumber(peakFrequency);
//            writeString("F:");
//            writeString("\n\r");
            

        }
        else
        {
////            writeString("\033[2J"); // Clear terminal
//            writeString("0:");
////            ScaleSignal(&BufferA0_regs[0]);
//            writeString("1:");
////            ScaleSignal(&BufferA1_regs[0]);
//            writeString("G:");
//            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferA0_regs[0],
//                         &twiddleFactors[0], COEFFS_IN_DATA);
//            writeString("H:");
//            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferA1_regs[0],
//                         &twiddleFactors[0], COEFFS_IN_DATA);
//
//            writeString("I:");
//
//            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &BufferB0_regs[0], &squaredOutput[0]);
//            writeString("J1:");
//            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin0);
//            writeString("J2:");
//            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &BufferB1_regs[0], &squaredOutput[0]);
//            writeString("J3:");
//            /* Find the frequency Bin ( = index into the SigCmpx[] array) that has the largest energy*/
//            /* i.e., the largest spectral component */
//            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin1);
//
//            writeString("K:");
//            /* Compute the frequency (in Hz) of the largest spectral component */
//            peakFrequency = peakFrequencyBin0*(SAMPLING_RATE/FFT_BLOCK_LENGTH);
//            writeNumber(peakFrequency);
//            writeString(":");
//            peakFrequency = peakFrequencyBin1*(SAMPLING_RATE/FFT_BLOCK_LENGTH);
//            writeNumber(peakFrequency);
//            writeString("L:");
//            writeString("\n\r");
            
        }
    }
    
}
