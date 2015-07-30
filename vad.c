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
    static UINT32 FrameEnergy = 0;
    static UINT32 DynamicThreshold = 0;
    static UINT8 firstTime = 1;
    INT16 tempVal = 0;
    INT16 tempSubVal = 0;
//    INTCON2bits.GIE = 0; // Disable global interrupts
    
    
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

        tempSubVal = BufferA0_regs[counter].real - HALF_ADC_OFFSET;
        tempVal = (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferA0_regs[counter].real;
        tempSubVal = BufferA1_regs[counter].real - HALF_ADC_OFFSET;
        tempVal = (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferA1_regs[counter].real;
        tempSubVal = BufferA2_regs[counter].real - HALF_ADC_OFFSET;
        tempVal = (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferA2_regs[counter].real;
        tempSubVal = BufferA3_regs[counter].real - HALF_ADC_OFFSET;
        tempVal = (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferA3_regs[counter].real;
        tempVal = tempVal >> 2; // Divided by 4

        BufferA0_regs[counter].real = (BufferA0_regs[counter].real - HALF_ADC_OFFSET) << 6;
        BufferA1_regs[counter].real = (BufferA1_regs[counter].real - HALF_ADC_OFFSET) << 6;
        BufferA2_regs[counter].real = (BufferA2_regs[counter].real - HALF_ADC_OFFSET) << 6;
        BufferA3_regs[counter].real = (BufferA3_regs[counter].real - HALF_ADC_OFFSET) << 6;

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

        tempSubVal = BufferB0_regs[counter].real - HALF_ADC_OFFSET;
        tempVal = (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferB0_regs[counter].real;
        tempSubVal = BufferB1_regs[counter].real - HALF_ADC_OFFSET;
        tempVal = (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferB1_regs[counter].real;
        tempSubVal = BufferB2_regs[counter].real - HALF_ADC_OFFSET;
        tempVal = (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferB2_regs[counter].real;
        tempSubVal = BufferB3_regs[counter].real - HALF_ADC_OFFSET;
        tempVal = (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferB3_regs[counter].real;
        tempVal = tempVal >> 2; // Divided by 4

        BufferB0_regs[counter].real = (BufferB0_regs[counter].real - HALF_ADC_OFFSET) << 6;
        BufferB1_regs[counter].real = (BufferB1_regs[counter].real - HALF_ADC_OFFSET) << 6;
        BufferB2_regs[counter].real = (BufferB2_regs[counter].real - HALF_ADC_OFFSET) << 6;
        BufferB3_regs[counter].real = (BufferB3_regs[counter].real - HALF_ADC_OFFSET) << 6;

        FrameEnergy += tempVal;
        
    }
    
    counter++;
    if(counter == FFT_BLOCK_LENGTH)
    {
        activeBuffer ^= 1;
        activeBuffer &= 0x1;

//        if (firstTime)
//        {
//            DynamicThreshold = FrameEnergy + 2000;
//            firstTime = 0;
//        }
//        else
//        {
//            DynamicThreshold = DynamicThreshold + (( (FrameEnergy + 1000) - DynamicThreshold ) >> 5);
//            if (DynamicThreshold > 50000)
//                DynamicThreshold = 50000;
//            if (DynamicThreshold < 33000)
//                DynamicThreshold = 33000;
//        }

//        FrameEnergy = FrameEnergy >> LOG2_BLOCK_LENGTH; // Divide by BLOCK_LENGTH
//        writeString("NRGY:");
//        writeRoundFloatNum(FrameEnergy);
//        writeString(" TH:");
//        writeRoundFloatNum(DynamicThreshold);
//        writeString("\n\r");

//        if(FrameEnergy > DynamicThreshold)
        if(FrameEnergy > 35500 )
        {
            PORTBbits.RB15 = 1;
            adcService();
            
        }
        else
        {
            // Do Calibration only in frames with no voice
            PORTBbits.RB15 = 0;
            
        }

        FrameEnergy = 0;

    }

    counter = counter % FFT_BLOCK_LENGTH;

//    INTCON2bits.GIE = 1; // Enable global interrupts
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
    
    INT32 peakFrequency = 0;    /* frequency of the largest spectral component */
    INT16 squaredOutput[FFT_BLOCK_LENGTH / 2] = {0};
    fractcomplex results[FFT_BLOCK_LENGTH];
//    PORTBbits.RB15 ^= 1;
//    writeString("x");

    TwidFactorInit (LOG2_BLOCK_LENGTH, &twiddleFactors[0], 0);
    
    if (activeBuffer == 0)
    {

            FFTComplex(LOG2_BLOCK_LENGTH, &BufferB2_regs[0].real, &BufferB0_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);
            
            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferB1_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferB2_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferB3_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &BufferB0_regs[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin0);
            
            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &BufferB1_regs[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

        /* Find the frequency Bin ( = index into the SigCmpx[] array) that has the largest energy*/
        /* i.e., the largest spectral component */
            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin1);
//            /* Compute the frequency (in Hz) of the largest spectral component */
            peakFrequency = FFT_BINS[peakFrequencyBin0];
            writeString("B:");
            writeNumber(peakFrequency);
            writeString(" ");
            peakFrequency = FFT_BINS[peakFrequencyBin1];
            writeNumber(peakFrequency);
            writeString("\n\r");

    }
    else
    {

            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferA0_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferA1_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);
            
            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferA2_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            FFTComplexIP(LOG2_BLOCK_LENGTH, &BufferA3_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &BufferA0_regs[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin0);

            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &BufferA1_regs[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

            /* Find the frequency Bin ( = index into the SigCmpx[] array) that has the largest energy*/
            /* i.e., the largest spectral component */
            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin1);

//            /* Compute the frequency (in Hz) of the largest spectral component */
            peakFrequency = FFT_BINS[peakFrequencyBin0];
            writeString("A:");
            writeNumber(peakFrequency);
            writeString(" ");
            peakFrequency = FFT_BINS[peakFrequencyBin1];
            writeNumber(peakFrequency);
            writeString("\n\r");

    }
    
    
}
