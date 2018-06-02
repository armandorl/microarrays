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
#include <stdbool.h>       /* Includes true/false definition                  */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp              */
#include "serialDriver.h"
#include "dsp.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

BufferType BufferA;
BufferType BufferB;


fractcomplex Buffer0_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far)) = {{0}};
fractcomplex Buffer1_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far)) = {{0}};
fractcomplex Buffer2_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far)) = {{0}};
fractcomplex Buffer3_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far)) = {{0}};

fractcomplex Buffer_results1[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far));
fractcomplex Buffer_results2[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far));

fractcomplex twiddleFactors[FFT_BLOCK_LENGTH/2] 	/* Declare Twiddle Factor array in X-space*/
__attribute__ ((section (".xbss, bss, xmemory"), aligned (FFT_BLOCK_LENGTH*2)));

fractional hammingWindow[FFT_BLOCK_LENGTH];

INT8 currentFrame = 0;
INT8 startService = 0;
INT8 activeBuffer = 0;

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/


INT16 main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();
    InitSerial();

    /* We need to do this only once at start-up */
    

    writeString("Micro Arrays  VER.1.00\n\r");

    /* Init constants for FFT */
    TwidFactorInit (LOG2_BLOCK_LENGTH, &twiddleFactors[0], 0);
    
    HammingInit(FFT_BLOCK_LENGTH, &hammingWindow[0]);
    AD1CON1bits.ADON = 1;

    /* calibration(); */
    
    while(1)
    {
    }

}
