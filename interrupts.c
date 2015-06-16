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

    storeValues();
    
}
