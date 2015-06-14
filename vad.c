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

UINT16 EnergyCh[ADC_CHANNELS][CHANNEL_ENERGY_ARRAY] __attribute__((space(auto_psv))) = {{0}};

//__eds__ int __attribute__((space(xmemory),eds))
//fractcomplex micSigCmpx[ADC_CHANNELS][FFT_BLOCK_LENGTH] __attribute__((space(auto_psv)));
fractcomplex micSigCmpx[ADC_CHANNELS][FFT_BLOCK_LENGTH] _YDATA(FFT_BLOCK_LENGTH * 2 * 2);

INT16 _PERSISTENT CHANNEL_OFFSET[ADC_CHANNELS];
INT16 _PERSISTENT CHANNEL_GAIN[ADC_CHANNELS];
UINT8 _PERSISTENT CALIBRATION_AVAILABLE;

INT16 ProcessADCSamples(INT16 *signal, UINT8 channel)
{

    /* Remove the signal offset */
    
    

    /* Detect if voice is available */

    return 0;

}

void CalculateAverage(INT16 *signal, UINT8 channel)
{

    if ( CALIBRATION_AVAILABLE == 0xAD )
    {
        // Calibration is available so skip it

    }
    else
    {
        // Calibration only required once
        CHANNEL_OFFSET[channel] = ( CHANNEL_OFFSET[channel] +
                                    Calibrate(signal)          ) / 2;
      
    }

}

void adcService(void)
{
    static UINT8 DmaBuffer = 0;
    static UINT16 initCounter = 0;
    static UINT8 StartFlag = 0;
    PORTBbits.RB15 ^= 1;
    writeString(".");
    if(StartFlag == 1)
    {
        if (DmaBuffer == 1)
        {

            // Register B has just been written so it can be processed
            EnergyCh[0][0] = ProcessADCSamples(&BufferB_regs.channel[0][0], 0);
            EnergyCh[1][0] = ProcessADCSamples(&BufferB_regs.channel[1][0], 1);
            EnergyCh[2][0] = ProcessADCSamples(&BufferB_regs.channel[2][0], 2);
            EnergyCh[3][0] = ProcessADCSamples(&BufferB_regs.channel[3][0], 3);

        }
        else
        {

            EnergyCh[0][0] = ProcessADCSamples(&BufferA_regs.channel[0][0], 0);
            EnergyCh[1][0] = ProcessADCSamples(&BufferA_regs.channel[1][0], 1);
            EnergyCh[2][0] = ProcessADCSamples(&BufferA_regs.channel[2][0], 2);
            EnergyCh[3][0] = ProcessADCSamples(&BufferA_regs.channel[3][0], 3);

        }
    }
    else
    {

        if (initCounter > CALIBRATION_START && initCounter <= CALIBRATION_END)
        {
            if (DmaBuffer == 1)
            {
                CalculateAverage(&BufferB_regs.channel[0][0], 0);
                CalculateAverage(&BufferB_regs.channel[1][0], 1);
                CalculateAverage(&BufferB_regs.channel[2][0], 2);
                CalculateAverage(&BufferB_regs.channel[3][0], 3);
            }
            else
            {
                CalculateAverage(&BufferA_regs.channel[0][0], 0);
                CalculateAverage(&BufferA_regs.channel[1][0], 1);
                CalculateAverage(&BufferA_regs.channel[2][0], 2);
                CalculateAverage(&BufferA_regs.channel[3][0], 3);
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
            writeString("\n\rMicrophone 3 offset:\n\r");
            writeNumber(CHANNEL_OFFSET[2]);
            writeString("\n\rMicrophone 4 offset:\n\r");
            writeNumber(CHANNEL_OFFSET[3]);
            writeString("\n\rCalibration end\n\r");
        }else
        {
            initCounter++;
            if (initCounter == CALIBRATION_START )
            {
                writeString("Calibration started");
            }
        }
        
    }
    DmaBuffer ^= 1;

}
