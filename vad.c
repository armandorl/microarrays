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
/*
 * 
 */


INT8 activeBuffer = 0;
UINT16 EnergyCh[ADC_CHANNELS][CHANNEL_ENERGY_ARRAY];

//__eds__ int __attribute__((space(xmemory),eds))
//fractcomplex micSigCmpx[ADC_CHANNELS][FFT_BLOCK_LENGTH] __attribute__((space(auto_psv)));
//fractcomplex micSigCmpx[ADC_CHANNELS][FFT_BLOCK_LENGTH] _YDATA(ADC_CHANNELS*FFT_BLOCK_LENGTH);

INT32 _PERSISTENT CHANNEL_OFFSET[ADC_CHANNELS];
INT16 _PERSISTENT CHANNEL_GAIN[ADC_CHANNELS];
UINT8 _PERSISTENT CALIBRATION_AVAILABLE;

void storeValues(void)
{
    static UINT16 counter = 0;
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
    CHANNEL_OFFSET[channel] = ( CHANNEL_OFFSET[channel] +
                                Calibrate(signal) ) / 2;
      


}

void adcService(void)
{
    static UINT16 initCounter = 0;
    static UINT8 StartFlag = 0;
    PORTBbits.RB15 ^= 1;
    writeString(".");
    
    if(StartFlag == 1)
    {
        if (activeBuffer == 1)
        {

            // Register B has just been written so it can be processed
            writeString("A");
            EnergyCh[0][0] = ProcessADCSamples(&BufferB_regs[0][0], 0);
            EnergyCh[1][0] = ProcessADCSamples(&BufferB_regs[1][0], 1);
            

        }
        else
        {
            writeString("B");
            EnergyCh[0][0] = ProcessADCSamples(&BufferA_regs[0][0], 0);
            EnergyCh[1][0] = ProcessADCSamples(&BufferA_regs[1][0], 1);
            

        }
    }
    else
    {

        if (initCounter > CALIBRATION_START && initCounter <= CALIBRATION_END)
        {
            if (activeBuffer == 1)
            {
                CalculateAverage(&BufferB_regs[0][0], 0);
                CalculateAverage(&BufferB_regs[1][0], 1);
                
            }
            else
            {
                CalculateAverage(&BufferA_regs[0][0], 0);
                CalculateAverage(&BufferA_regs[1][0], 1);
                
            }
            writeString(".");
            initCounter++;
        }
        else if (initCounter > CALIBRATION_END)
        {
            CALIBRATION_AVAILABLE = 0xAD;
            StartFlag=1;
            writeString("\n\rMicrophone 1 offset:\n\r");
            writeNumber(CHANNEL_OFFSET[0]);
            writeString("\n\rMicrophone 2 offset:\n\r");
            writeNumber(CHANNEL_OFFSET[1]);
        
        }else
        {
            initCounter++;
            if (initCounter == CALIBRATION_START )
            {
                CHANNEL_OFFSET[0] = 0x200;
                CHANNEL_OFFSET[1] = 0x200;
                writeString("Calibration started");
            }
        }
        
    }

}
