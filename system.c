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


#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */

#include "system.h"          /* variables/params used by system.c             */

/******************************************************************************/
/* System Level Functions                                                     */
/*                                                                            */
/* Custom oscillator configuration funtions, reset source evaluation          */
/* functions, and other non-peripheral microcontroller initialization         */
/* functions get placed in system.c.                                          */
/*                                                                            */
/******************************************************************************/

/* Refer to the device Family Reference Manual Oscillator section for
information about available oscillator configurations.  Typically
this would involve configuring the oscillator tuning register or clock
switching useing the compiler's __builtin_write_OSCCON functions.
Refer to the C Compiler for PIC24 MCUs and dsPIC DSCs User Guide in the
compiler installation directory /doc folder for documentation on the
__builtin functions.*/

/* TODO Add clock switching code if appropriate.  An example stub is below.   */
void ConfigureOscillator(void)
{
        // Watchdog disabled
        RCONbits.SWDTEN = 0;

        /* Fcy = Fosc / 2 */
        CLKDIVbits.FRCDIV = 0;

        CLKDIVbits.DOZEN = 0;
        CLKDIVbits.DOZE = 0;

        PLLFBDbits.PLLDIV = 40;
        CLKDIVbits.PLLPRE = 0;
        CLKDIVbits.PLLPOST = 0;
        
        /* Set deterministic 13 instruction interrupt overhead */
        CORCONbits.VAR = 0;
        CORCONbits.IPL3 = 0;
        
        /* Interrup nesting enabled */
        INTCON1bits.NSTDIS = 0;
        
}

