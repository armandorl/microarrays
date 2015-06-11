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

#include "user.h"       /* Includes true/false definition */

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

void __attribute__((__interrupt__,no_auto_psv)) _AD1Interrupt(void){
    
    
    // Restart timer
    PORTBbits.RB14 ^= 1;
    IFS0bits.AD1IF = 0; // ADC Interrupt flag

}

void __attribute__((__interrupt__,no_auto_psv)) _DMA0Interrupt(void){
    // Switch between Primary and Secondary Ping-Pong buffers
    // Period 3.06 ms with no calculations
    // Period 28.12ms with calculations
//    adcService();
    IFS0bits.DMA0IF = 0;// Clear the DMA5 Interrupt Flag
    
}