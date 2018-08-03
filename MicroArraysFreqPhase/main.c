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

    /* We need to do this only once at start-up */
    writeString("Micro Arrays  VER.1.00\n\r");

    writeNumber(0x12345678);
    writeString("\n\r");
    writeNumber(0xabcd1234);
    writeString("\n\r");
    writeNumber(100);
    writeString("\n\r");
    
    /* Init constants for FFT */
    TwidFactorInit (LOG2_BLOCK_LENGTH, &twiddleFactors[0], 0);
#if defined(HANNING_WINDOW) /* Hanning window disabled */
    /* Init hanning window constants */
    HanningInit(FFT_BLOCK_LENGTH, &hanningWindow[0]);
#endif

    /* Initialize ADC */
    AD1CON1bits.ADON = 1;

    /* calibration(); */
    
    while(1)
    {
        /* Do nothing flow is controlled by DMA interrupts */
    }

}
