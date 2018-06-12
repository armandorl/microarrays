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
#include <string.h>
#include <math.h>
#include <dsp.h>
#include <libq.h>

#include "fft.h"
#include "user.h"
#include "serialDriver.h"

#define CALIBRATION_START 100  // Value * 100ms cycle
#define CALIBRATION_END   1000

#define HALF_ADC_OFFSET       512

UINT8 StartFlag = 0;
#if 0
static UINT32 FrameEnergyChan[4] = {0};
#endif
/*
 * 
 */

void storeValues(void)
{
#if 0
    static UINT16 counter = 0;
    static UINT32 FrameEnergy = 0;
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
        FrameEnergyChan[0] += tempVal;

        tempSubVal = BufferA1_regs[counter].real - HALF_ADC_OFFSET;
        FrameEnergyChan[1] += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferA1_regs[counter].real;
        tempVal += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferA1_regs[counter].real;

        tempSubVal = BufferA2_regs[counter].real - HALF_ADC_OFFSET;
        FrameEnergyChan[2] += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferA2_regs[counter].real;
        tempVal += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferA2_regs[counter].real;

        tempSubVal = BufferA3_regs[counter].real - HALF_ADC_OFFSET;
        FrameEnergyChan[3] += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferA3_regs[counter].real;
        tempVal += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferA3_regs[counter].real;

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
        FrameEnergyChan[0] += tempVal;

        tempSubVal = BufferB1_regs[counter].real - HALF_ADC_OFFSET;
        FrameEnergyChan[1] += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferB1_regs[counter].real;
        tempVal += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferB1_regs[counter].real;

        tempSubVal = BufferB2_regs[counter].real - HALF_ADC_OFFSET;
        FrameEnergyChan[2] += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferB2_regs[counter].real;
        tempVal += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferB2_regs[counter].real;

        tempSubVal = BufferB3_regs[counter].real - HALF_ADC_OFFSET;
        FrameEnergyChan[3] += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferB3_regs[counter].real;
        tempVal += (tempSubVal < 0)? HALF_ADC_OFFSET - tempSubVal : BufferB3_regs[counter].real;

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
        if(FrameEnergy > 142000 )
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
        FrameEnergyChan[0] = 0;
        FrameEnergyChan[1] = 0;
        FrameEnergyChan[2] = 0;
        FrameEnergyChan[3] = 0;

    }

    counter = counter % FFT_BLOCK_LENGTH;

//    INTCON2bits.GIE = 1; // Enable global interrupts
    IFS0bits.AD1IF = 0; // ADC Interrupt flag
    PORTBbits.RB14 ^= 1;
#endif
}

void InitADCSignals(BufferType *inBuffer)
{
    int i;
    float myarray[1024];
#if 0
    /* Apply hamming window to all signals */
    VectorWindow(FFT_BLOCK_LENGTH, 
                 &inBuffer->Adc1Ch0[0], 
                 &inBuffer->Adc1Ch0[0], 
                 &hanningWindow[0]);
    
    VectorWindow(FFT_BLOCK_LENGTH, 
                 &inBuffer->Adc1Ch1[0], 
                 &inBuffer->Adc1Ch1[0], 
                 &hanningWindow[0]);
    
    VectorWindow(FFT_BLOCK_LENGTH, 
                 &inBuffer->Adc1Ch2[0], 
                 &inBuffer->Adc1Ch2[0], 
                 &hanningWindow[0]);
    
    VectorWindow(FFT_BLOCK_LENGTH, 
                 &inBuffer->Adc1Ch3[0], 
                 &inBuffer->Adc1Ch3[0], 
                 &hanningWindow[0]);
#endif 
    for(i=0; i < 512; i++)
    {
        INTCON2bits.GIE = 0; // Enable global interrupts
        myarray[i] = _itofQ15(i<<6);
        INTCON2bits.GIE = 1; // Enable global interrupts
    }
    
    for(i=512; i < 1024; i++)
    {
        INTCON2bits.GIE = 0; // Enable global interrupts
        myarray[i] = _itofQ15((i<<6)|0x8000);
        INTCON2bits.GIE = 1; // Enable global interrupts
    }
    /* Convert signals to complex */
    for(i=0; i < FFT_BLOCK_LENGTH; i++)
    {
        Buffer0_regs[i].real = inBuffer->Adc1Ch0[i];
        Buffer1_regs[i].real = inBuffer->Adc1Ch1[i];
        Buffer2_regs[i].real = inBuffer->Adc1Ch2[i];
        Buffer3_regs[i].real = inBuffer->Adc1Ch3[i];
        
        Buffer0_regs[i].imag = 0;
        Buffer1_regs[i].imag = 0;
        Buffer2_regs[i].imag = 0;
        Buffer3_regs[i].imag = 0;
    }
 

}


INT16 ProcessADCSamples(fractcomplex *signal1, fractcomplex *signal2)
{
    INT32 peakFrequency = 0;    /* frequency of the largest spectral component */
    INT16 peakFrequencyBin = 0;
    INT16 squaredOutput[FFT_BLOCK_LENGTH / 2] = {0};
    fractional phaseSig1 = 0;
    fractional phaseSig2 = 0;
    FLOAT32 phaseFl1 = 0.0f;
    FLOAT32 phaseFl2 = 0.0f;
    FLOAT32 diffPhase = 0.0f;

    /* Remove the signal offset */
    FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results1[0], signal1,
                 &twiddleFactors[0], COEFFS_IN_DATA);

    SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results1[0], &squaredOutput[0]);

    VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin);
    
    phaseSig1 = _Q15atanYByXByPI(Buffer_results1[peakFrequencyBin].imag, 
                     Buffer_results1[peakFrequencyBin].real);
    
    FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results1[0], signal2,
                 &twiddleFactors[0], COEFFS_IN_DATA);

    SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results1[0], &squaredOutput[0]);

    VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin);
    
    phaseSig2 = _Q15atanYByXByPI(Buffer_results1[peakFrequencyBin].imag, 
                     Buffer_results1[peakFrequencyBin].real);

    phaseFl1 = _itofQ15(phaseSig1) * 180.0f;
    phaseFl2 = _itofQ15(phaseSig2) * 180.0f;
    diffPhase = phaseFl2 - phaseFl1;
    
    writeNumber((INT32)diffPhase);
    writeString("\n");
    /* Detect if voice is available */

    return (INT16)peakFrequency;

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
#if 0
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
#endif
}


void adcService(void)
{
#if 0   
    INT32 peakFrequency = 0;    /* frequency of the largest spectral component */
    INT16 peakFrequencyBin = 0;
    INT16 squaredOutput[FFT_BLOCK_LENGTH / 2] = {0};

    TwidFactorInit (LOG2_BLOCK_LENGTH, &twiddleFactors[0], 0);
    
    if (activeBuffer == 0)
    {

         // Buffer 0
            FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results[0].real, &BufferB0_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);
            
            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin);

            writeString("B0:");
            writeNumber(FrameEnergyChan[0]);
            writeString(" ");
            peakFrequency = FFT_BINS[peakFrequencyBin];
            writeNumber(peakFrequency);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].real);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].imag);

            // Buffer 1
            FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results[0].real, &BufferB1_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results[0].real, &squaredOutput[0]);
            
            squaredOutput[0] = 0; // Remove low frequencies

            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin);

            writeString(" B1:");
            writeNumber(FrameEnergyChan[1]);
            writeString(" ");
            peakFrequency = FFT_BINS[peakFrequencyBin];
            writeNumber(peakFrequency);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].real);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].imag);

            // Buffer 2
            FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results[0].real, &BufferB2_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin);

            writeString(" B2:");
            writeNumber(FrameEnergyChan[2]);
            writeString(" ");
            peakFrequency = FFT_BINS[peakFrequencyBin];
            writeNumber(peakFrequency);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].real);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].imag);

            // Buffer 3
            FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results[0].real, &BufferB3_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin);

            writeString(" B3:");
            writeNumber(FrameEnergyChan[3]);
            writeString(" ");
            peakFrequency = FFT_BINS[peakFrequencyBin];
            writeNumber(peakFrequency);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].real);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].imag);
            writeString("\n\r");

    }
    else
    {

         // Buffer 0
            FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results[0].real, &BufferA0_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin);

            writeString("A0:");
            writeNumber(FrameEnergyChan[0]);
            writeString(" ");
            peakFrequency = FFT_BINS[peakFrequencyBin];
            writeNumber(peakFrequency);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].real);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].imag);

            // Buffer 1
            FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results[0].real, &BufferA1_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin);

            writeString(" A1:");
            writeNumber(FrameEnergyChan[1]);
            writeString(" ");
            peakFrequency = FFT_BINS[peakFrequencyBin];
            writeNumber(peakFrequency);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].real);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].imag);

            // Buffer 2
            FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results[0].real, &BufferA2_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin);

            writeString(" A2:");
            writeNumber(FrameEnergyChan[2]);
            writeString(" ");
            peakFrequency = FFT_BINS[peakFrequencyBin];
            writeNumber(peakFrequency);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].real);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].imag);

            // Buffer 3
            FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results[0].real, &BufferA3_regs[0].real,
                         &twiddleFactors[0].real, COEFFS_IN_DATA);

            SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results[0].real, &squaredOutput[0]);

            squaredOutput[0] = 0; // Remove low frequencies

            VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin);

            writeString(" A3:");
            writeNumber(FrameEnergyChan[3]);
            writeString(" ");
            peakFrequency = FFT_BINS[peakFrequencyBin];
            writeNumber(peakFrequency);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].real);
            writeString(" ");
            writeNumber(Buffer_results[peakFrequencyBin].imag);
            writeString("\n\r");

    }
#endif
    
}
