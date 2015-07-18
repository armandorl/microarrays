
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

#include "user.h"
#include "serialDriver.h"

INT16 writeString(INT8 * text)
{
    UINT8 i = 0;
    UINT8 ret = 0;
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

INT16 writeNumber(INT16 value)
{
    UINT8 ret = 0;
    INT8 number[10] = {0};
    INT8 *printVal = &number[0];
    
    if(value < 0)
    {
        writeString("-");
        value = value * -1;
    }

    if( value < 1000000)
    {
        *printVal = '0' + (value / 100000);
        value = value % 100000;
        *(printVal+1) = '0' + (value / 10000);
        value = value % 10000;
        *(printVal+2) = '0' + (value / 1000);
        value = value % 1000;
        *(printVal+3) = '0' + (value / 100);
        value = value % 100;
        *(printVal+4) = '0' + (value / 10);
        value = value % 10;
        *(printVal+5) = '0' + value;
        
        writeString(printVal);
    }
    else
    {
        writeString("Only values that are less than 1000000 are supported.");
        ret = 1;
    }
    

    return ret;

}

INT16 writeRoundFloatNum(FLOAT32 value)
{
    UINT8 ret = 0;
    INT8 number[10] = {0};
    INT8 *printVal = &number[0];

    if(value < 0)
    {
        writeString("-");
        value = value * -1;
    }

    if( value < 1000000)
    {
        *printVal = '0' + (value / 100000);
        value = (INT32)value % 100000;
        *(printVal+1) = '0' + (value / 10000);
        value = (INT32)value % 10000;
        *(printVal+2) = '0' + (value / 1000);
        value = (INT32)value % 1000;
        *(printVal+3) = '0' + (value / 100);
        value = (INT32)value % 100;
        *(printVal+4) = '0' + (value / 10);
        value = (INT32)value % 10;
        *(printVal+5) = '0' + value;

        writeString(printVal);
    }
    else
    {
        writeString("Only values that are less than 1000000 are supported.");
        ret = 1;
    }


    return ret;

}

INT8 getCharacter(void)
{
    char ReceivedChar = 0;

    /* Check for receive errors */
    if(U1STAbits.FERR == 1)
    {
        //continue;
    }

    /* Must clear the overrun error to keep UART receiving */
    if(U1STAbits.OERR == 1)
    {
        U1STAbits.OERR = 0;
        //continue;
    }

    /* Get the data */
    if(U1STAbits.URXDA == 1)
    {
        while(U1STAbits.URXDA == 1){
            ReceivedChar = U1RXREG;
        }
    }
    
    return ReceivedChar;
    
}
