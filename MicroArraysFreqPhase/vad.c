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
#include <libq.h>

#include "fft.h"
#include "globals.h"
#include "serialDriver.h"


UINT8 StartFlag = 0;
#if 0
static UINT32 FrameEnergyChan[4] = {0};
#endif
/*
 * 
 */

void InitADCSignals(BufferType *inBuffer)
{
    int i;
#if defined(HANNING_WINDOW) /* Hanning window disabled */
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
    INT16 peakFrequencyBin = 0;
    INT16 peakFrequencyBin1 = 0;
    INT16 peakFrequencyBin2 = 0;
    INT16 squaredOutput[FFT_BLOCK_LENGTH / 2] = {0};
    fractional phaseSig1 = 0;
    fractional phaseSig2 = 0;
    FLOAT32 phaseFl1 = 0.0f;
    FLOAT32 phaseFl2 = 0.0f;
    FLOAT32 diffPhase = 0.0f;

    /* Signal 1 */
    /* Calculate the FFT */
    FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results1[0], signal1,
                 &twiddleFactors[0], COEFFS_IN_DATA);

    /* Calculate the magnitude */
    SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results1[0], &squaredOutput[0]);

    /* Get the index of the greatest magnitude */
    VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin1);
    
    /* Get the phase in the max magnitude bin */
    phaseSig1 = _Q15atanYByXByPI(Buffer_results1[peakFrequencyBin1].imag, 
                     Buffer_results1[peakFrequencyBin1].real);
    
    /* Signal 2 */
    /* Calculate the FFT */
    FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results1[0], signal2,
                 &twiddleFactors[0], COEFFS_IN_DATA);

    /* Calculate the magnitude */
    SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results1[0], &squaredOutput[0]);

    /* Get the index of the greatest magnitude */
    VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], &peakFrequencyBin2);
    
    /* Get the phase in the max magnitude bin */
    phaseSig2 = _Q15atanYByXByPI(Buffer_results1[peakFrequencyBin2].imag, 
                     Buffer_results1[peakFrequencyBin2].real);

    /* Only calculate difference in phases if the same freq bin is at Max value */
    /* If different bins are found it is possible that the signals are noisy or */
    /* one microphone detected another signal with greater energy which would   */
    /* provide an invalid calculation */
    writeString("F1:");
    writeNumber(peakFrequencyBin1);
    writeString(" F2:");
    writeNumber(peakFrequencyBin2);
    
    if(peakFrequencyBin1 == peakFrequencyBin2)
    {
        peakFrequencyBin = 0;
        INTCON2bits.GIE = 0; // Disable global interrupts
        phaseFl1 = _itofQ15(phaseSig1) * 180.0f;
        phaseFl2 = _itofQ15(phaseSig2) * 180.0f;
        INTCON2bits.GIE = 1; // Enable global interrupts
        diffPhase = phaseFl2 - phaseFl1;

        writeString(" ");
        writeNumber((INT32)phaseFl2);
        writeString(" ");
        writeNumber((INT32)phaseFl1);
        writeString("\n");
    }

    return (INT16)peakFrequencyBin;

}
