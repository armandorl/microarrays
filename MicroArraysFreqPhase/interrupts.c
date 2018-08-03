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

#include <stdint.h>        /* Includes uint16_t definition   */
#include <stdbool.h>

#include "vad.h"       /* Includes true/false definition */
#include "globals.h"
/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

void __attribute__((__interrupt__,no_auto_psv)) _AD1Interrupt(void){
    IFS0bits.AD1IF = 0; // ADC Interrupt flag
    /*storeValues();*/
    
}


void __attribute__((__interrupt__,no_auto_psv)) _DMA0Interrupt(void){
    PORTBbits.RB14 ^= 1;
    IFS0bits.DMA0IF = 0;  // Clear the DMA Interrupt flag bit
    
    if(((volatile unsigned int)DMAPPS) & 0x01)
    {
        PORTBbits.RB15 ^= 1;
        
        InitADCSignals(&BufferA);

        /* When buffer B is selected process Buffer A */
        ProcessADCSamples(&Buffer0_regs[0], &Buffer1_regs[0]);

        PORTBbits.RB15 ^= 1;
    }
    else
    {
        PORTBbits.RB15 ^= 1;
        /* When buffer A is selected process Buffer B */
        InitADCSignals(&BufferB);

        /* When buffer B is selected process Buffer A */
        ProcessADCSamples(&Buffer0_regs[0], &Buffer1_regs[0]);

        PORTBbits.RB15 ^= 1;
        
    }
}