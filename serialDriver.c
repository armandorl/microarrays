
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
    char number[5] = {0};
    char *printVal = &number[0];
    
    if(value < 0)
    {
        writeString("-");
        value = value * -1;
    }

    if( value < 10000)
    {
        *printVal = '0' + (value / 1000);
        value = value % 1000;
        *(printVal+1) = '0' + (value / 100);
        value = value % 100;
        *(printVal+2) = '0' + (value / 10);
        value = value % 10;
        *(printVal+3) = '0' + value;
        
        writeString(printVal);
    }
    else
    {
        writeString("Only values that are less than 10000 are supported.");
    }
    

    return ret;

}

