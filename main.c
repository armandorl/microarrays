/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

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


#include <stdint.h>        /* Includes uint16_t definition                    */
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "init.h"          /* User funct/params, such as InitApp              */
#include "fft.h"
#include "serialDriver.h"
#include "dsp.h"
#include "globals.h"
#include "vad.h"


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

BufferType BufferA;
BufferType BufferB;

/* Buffers used to hold complex input signals */
fractcomplex Buffer0_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far)) = {{0}};
fractcomplex Buffer1_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far)) = {{0}};
fractcomplex Buffer2_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far)) = {{0}};
fractcomplex Buffer3_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far)) = {{0}};

/* Buffers to hold fft results */
fractcomplex Buffer_results1[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far));
fractcomplex Buffer_results2[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far));

/* FFT Twiddle Factors */
fractcomplex twiddleFactors[FFT_BLOCK_LENGTH/2] 	/* Declare Twiddle Factor array in X-space*/
__attribute__ ((section (".xbss, bss, xmemory"), aligned (FFT_BLOCK_LENGTH*2)));

volatile char curr_buffer = 0;

#if defined(HANNING_WINDOW) /* Hanning window disabled */
/* Hanning window */
fractional hanningWindow[FFT_BLOCK_LENGTH];
#endif

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/


INT16 main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitDmaAdc();
    InitSerial();
    InitTimer3();
    InitRingBuffer();

    /* We need to do this only once at start-up */
    writeStringBlk("Micro Arrays  VER.1.00\n\r");

    writeNumberBlk(0x12345678);
    writeStringBlk("\n\r");
    writeNumberBlk(0xabcd1234);
    writeStringBlk("\n\r");
    writeNumberBlk(100);
    writeStringBlk("\n\r");
    
    /* Init constants for FFT */
    TwidFactorInit (LOG2_BLOCK_LENGTH, &twiddleFactors[0], 0);
#if defined(HANNING_WINDOW) /* Hanning window disabled */
    /* Init hanning window constants */
    HanningInit(FFT_BLOCK_LENGTH, &hanningWindow[0]);
#endif

    /* Initialize ADC */
    AD1CON1bits.ADON = 1;
    T3CONbits.TON = 1; // Start Timer
    
    /* calibration(); */
    
    while(curr_buffer == 0)
    {
        /* Wait for first DMA interrupt */
    }
    
    while(1)
    {
        if(curr_buffer == 1)
        {
            curr_buffer = 0;
            if(((volatile unsigned int)DMAPPS) & 0x01)
            {
                //PORTBbits.RB15 ^= 1;

                InitADCSignals(&BufferA);

                /* When buffer B is selected process Buffer A */
                ProcessADCSamples(&Buffer0_regs[0], 
                                  &Buffer1_regs[0],
                                  &Buffer2_regs[0],
                                  &Buffer3_regs[0]);

                //PORTBbits.RB15 ^= 1;
            }
            else
            {
                //PORTBbits.RB15 ^= 1;
                /* When buffer A is selected process Buffer B */
                InitADCSignals(&BufferB);

                /* When buffer B is selected process Buffer A */
                ProcessADCSamples(&Buffer0_regs[0], 
                                  &Buffer1_regs[0],
                                  &Buffer2_regs[0],
                                  &Buffer3_regs[0]);

                //PORTBbits.RB15 ^= 1;

            }
            
        }
        
        ringSendData();
    }
}
