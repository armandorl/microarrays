
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

#include "globals.h"
#include "serialDriver.h"

#define NUMSHIFT(A, B)    ((A & ((INT32)0xF << (B*4))) >> (B*4))

static INT8 getHexValue(INT8 value);

INT16 writeString(INT8 * text)
{
    UINT8 i = 0;
    while( *(text+i) != 0)
    {
        if (U1STAbits.TRMT) //1 = Transmit Shift Register is empty and transmit buffer is empty
        {
            U1TXREG = *(text+i);// Transmit one char
            i++;
        }
    }

    return i;
}

INT16 writeChar(INT8 text)
{
    UINT8 ret = 0;
    while( ret == 0)
    {
        if (U1STAbits.TRMT) //1 = Transmit Shift Register is empty and transmit buffer is empty
        {
            U1TXREG = text;// Transmit one char
            ret = 1;
        }
    }

    return ret;
}

INT16 writeNumber(INT32 value)
{
    UINT8 ret = 0;
    
    writeString("0x");

    writeChar(getHexValue(NUMSHIFT(value, 7)));
    writeChar(getHexValue(NUMSHIFT(value, 6)));
    writeChar(getHexValue(NUMSHIFT(value, 5)));
    writeChar(getHexValue(NUMSHIFT(value, 4)));
    writeChar(getHexValue(NUMSHIFT(value, 3)));
    writeChar(getHexValue(NUMSHIFT(value, 2)));
    writeChar(getHexValue(NUMSHIFT(value, 1)));
    writeChar(getHexValue(NUMSHIFT(value, 0)));
    
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

/* Input range is expected to be exlusive from 0-15, else 
  this function will truncate it */
static INT8 getHexValue(INT8 value)
{
    value = value & 0xF;
    INT8 retValue = '0' + value;
    if(value > 9)
    {
        retValue = 'A' + (value - 10);        
    }
    
    return retValue;
}
