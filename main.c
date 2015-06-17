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
INT16 BufferA_regs[ADC_CHANNELS][BLOCKSIZE];
INT16 BufferB_regs[ADC_CHANNELS][BLOCKSIZE];

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
    AD1CON1bits.ADON = 1;

    
    
    while(1)
    {
//        writeString("Hola Mundo!\n\r");
        if (startService == 1)
        {
            calibration();
            adcService();
            startService = 0;
        }
    }

}
