
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

#include "serialDriver.h"

int writeString(char * text)
{
    int i = 0;
    int ret = 0;
    while( *(text+i) != 0 && ret == 0)
    {
        if (U1STAbits.TRMT) //1 = Transmit Shift Register is empty and transmit buffer is empty
        {
            U1TXREG = *(text+i);// Transmit one char
            i++;
        }

        if(i > MAX_UART_CHARS)
        {
            ret = 1; // Max number of characters reached
        }


    }

    return ret;
}

int writeNumber(int value)
{
    int ret = 0;
    if(value < 0)
    {
        writeString("-");
        value = value * -1;
    }
    

    return ret;

}

