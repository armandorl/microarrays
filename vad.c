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
    
static fractional getPhaseAndMaxFreq(fractcomplex *signal1, INT16* peakFrequencyBin);
static fractional getPhaseAndMaxFreqCC(fractcomplex *signal, INT16* peakFrequencyBin, fractional* phase1);
static void printPhaseDiff(INT16 freq1, fractional phase1, fractional phase2);

static _Q15 oneEighty;
static INT16 freqRange[FFT_BLOCK_LENGTH / 2];
#if 0
static UINT32 FrameEnergyChan[4] = {0};
#endif
/*
 * 
 */

void InitADCconstants(void)
{
    int i=0;
    oneEighty = Q15(180.0);
    for(i=0; i < (FFT_BLOCK_LENGTH/2); i++)
    {
        freqRange[i] = SAMPLING_RATE * (1 / FFT_BLOCK_LENGTH);
    }
    
}


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
    
    VectorScale(FFT_BLOCK_LENGTH * 2, (fractional*)&Buffer0_regs[0], (fractional*)&Buffer0_regs[0], Q15(0.5));
    VectorScale(FFT_BLOCK_LENGTH * 2, (fractional*)&Buffer1_regs[0], (fractional*)&Buffer1_regs[0], Q15(0.5));
    VectorScale(FFT_BLOCK_LENGTH * 2, (fractional*)&Buffer2_regs[0], (fractional*)&Buffer2_regs[0], Q15(0.5));
    VectorScale(FFT_BLOCK_LENGTH * 2, (fractional*)&Buffer3_regs[0], (fractional*)&Buffer3_regs[0], Q15(0.5));
}


void ProcessADCSamples(fractcomplex *signal1, 
                       fractcomplex *signal2,
                       fractcomplex *signal3,
                       fractcomplex *signal4)
{
    fractional phaseSig1 = 0;
    fractional phaseSig2 = 0;
    fractional phaseSig3 = 0;
    fractional phaseSig4 = 0;
    
    INT16 peakFrequencyBin1 = 0;
    INT16 peakFrequencyBin2 = 0;
    INT16 peakFrequencyBin3 = 0;
    INT16 peakFrequencyBin4 = 0;

    /* Signal 1 is the base */
    phaseSig1 = getPhaseAndMaxFreq(signal1, &peakFrequencyBin1);
    /* Signal 2 */
    phaseSig2 = getPhaseAndMaxFreqCC(signal2, &peakFrequencyBin2, &phaseSig1);
    printPhaseDiff(peakFrequencyBin2, phaseSig1, phaseSig2);
    writeStringAsync("\t");
    
    /* Signal 3 */
    phaseSig3 = getPhaseAndMaxFreqCC(signal3, &peakFrequencyBin3, &phaseSig1);
    printPhaseDiff(peakFrequencyBin3, phaseSig1, phaseSig3);
    writeStringAsync("\t");
    
    /* Signal 4 */
    phaseSig4 = getPhaseAndMaxFreqCC(signal4, &peakFrequencyBin4, &phaseSig1);
    printPhaseDiff(peakFrequencyBin4, phaseSig1, phaseSig4);
    writeStringAsync("\n");

}

static fractional getPhaseAndMaxFreq(fractcomplex *signal, INT16* peakFrequencyBin)
{
    INT16 squaredOutput[FFT_BLOCK_LENGTH / 2] = {0};
    fractional phaseSig = 0;
 
    /* Calculate the FFT */
    FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results1[0], signal,
                 &twiddleFactors[0], COEFFS_IN_DATA);

    /* Calculate the magnitude */
    SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results1[0], &squaredOutput[0]);

    /* Get the index of the greatest magnitude */
    VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], peakFrequencyBin);
    /* Get the phase in the max magnitude bin */
    INTCON2bits.GIE = 0;
    phaseSig = _Q15atanYByXByPI(Buffer_results1[*peakFrequencyBin].real, 
                     Buffer_results1[*peakFrequencyBin].imag);
    INTCON2bits.GIE = 1;
    
    return phaseSig;
}

static fractional getPhaseAndMaxFreqCC(fractcomplex *signal, INT16* peakFrequencyBin, fractional* phase1)
{
    INT16 squaredOutput[FFT_BLOCK_LENGTH / 2] = {0};
    fractional phaseSig = 0;
    INT16 temp, i;
    fractcomplex temp_results[FFT_BLOCK_LENGTH/2];
 
    /* Calculate the FFT */
    FFTComplex(LOG2_BLOCK_LENGTH, &Buffer_results2[0], signal,
                 &twiddleFactors[0], COEFFS_IN_DATA);

    for(i=0; i < (FFT_BLOCK_LENGTH / 2); i++)
    {
        temp_results[i].real = Buffer_results2[i].real;
        temp_results[i].imag = Buffer_results2[i].imag;
    }
    BitReverseComplex(LOG2_BLOCK_LENGTH, &Buffer_results2[0]);
    
    temp = CORCONbits.IF;
    for(i=0; i < (FFT_BLOCK_LENGTH / 2); i++)
    {
        INTCON2bits.GIE = 0;
        Buffer_results2[i].imag = _Q15neg(Buffer_results2[i].imag);
        CORCONbits.IF = 0;
        Buffer_results2[i].real = Buffer_results1[i].real * Buffer_results2[i].real;
        Buffer_results2[i].imag = Buffer_results1[i].imag * Buffer_results2[i].imag;
        CORCONbits.IF = 1;
        INTCON2bits.GIE = 1;
    }
    CORCONbits.IF = temp;
    
    /* Calculate the magnitude */
    SquareMagnitudeCplx(FFT_BLOCK_LENGTH / 2, &Buffer_results2[0], &squaredOutput[0]);

    /* Get the index of the greatest magnitude */
    VectorMax(FFT_BLOCK_LENGTH/2, &squaredOutput[0], peakFrequencyBin);

    /* Get the phase in the max magnitude bin */
    INTCON2bits.GIE = 0;
    *phase1 = _Q15atanYByXByPI(Buffer_results1[*peakFrequencyBin].real, 
                     Buffer_results1[*peakFrequencyBin].imag);
    phaseSig = _Q15atanYByXByPI(temp_results[*peakFrequencyBin].real, 
                     temp_results[*peakFrequencyBin].imag);
    INTCON2bits.GIE = 1;
    
    return phaseSig;
}

static void printPhaseDiff(INT16 freq1, fractional phase1, fractional phase2)
{
    FLOAT32 phaseFl1 = 0.0f;
    FLOAT32 phaseFl2 = 0.0f;
    FLOAT32 diffPhase = 0.0f;

    INTCON2bits.GIE = 0; // Disable global interrupts
    phaseFl1 = _itofQ15(phase1) * 180.0f;
    phaseFl2 = _itofQ15(phase2) * 180.0f;
    INTCON2bits.GIE = 1; // Enable global interrupts

    diffPhase = phaseFl2 - phaseFl1;
    writeNumberAsync((INT32)freq1);
    writeStringAsync(";");
    writeNumberAsync((INT32)diffPhase);
}
