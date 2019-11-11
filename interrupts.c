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



extern volatile char curr_buffer;
/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

void __attribute__((__interrupt__,no_auto_psv)) _AD1Interrupt(void){
    IFS0bits.AD1IF = 0; // ADC Interrupt flag
    /*storeValues();*/
    
}


void __attribute__((__interrupt__,no_auto_psv)) _DMA0Interrupt(void){
    //PORTBbits.RB14 ^= 1;
    IFS0bits.DMA0IF = 0;  // Clear the DMA Interrupt flag bit
    curr_buffer = 1;
}

/* Example code for Timer1 ISR*/
void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void)
{
    TMR3 = 0x00; // Clear timer register
    /* Interrupt Service Routine code goes here */
    IFS0bits.T3IF = 0; //Clear Timer1 interrupt flag
}
