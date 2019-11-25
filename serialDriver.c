
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
#define RING_SIZE    (256)


static INT8 getHexValue(INT8 value);
static INT16 writeChar(INT8 text);
static void writeCharAsync(INT8 text);

static UINT16 ringBuff[RING_SIZE];
static UINT16* pRingTail;
static UINT16* pRingHead;

void InitRingBuffer(void)
{
    pRingTail = &ringBuff[0];
    pRingHead = &ringBuff[0];
}

void ringSendData(void)
{
    if(pRingHead != pRingTail)
    {
        if (0 == U1STAbits.UTXBF)
        {
            U1TXREG = *pRingTail;
            pRingTail++;
            /* Ring buffer tail wrap around */
            if( pRingTail > &ringBuff[RING_SIZE-1] )
                pRingTail = &ringBuff[0];
        }
    }
}


INT16 writeStringAsync(INT8 * text)
{
    UINT8 i = 0;
    while( *(text+i) != 0)
    {
        writeCharAsync(*(text+i));// Transmit one char
        i++;
    }
    return i;
}

INT16 writeStringBlk(INT8 * text)
{
    UINT8 i = 0;
    while( *(text+i) != 0)
    {
        if (0 == U1STAbits.UTXBF) //0 = FIFO is not full
        {
            U1TXREG = *(text+i);// Transmit one char
            i++;
        }
    }

    return i;
}

INT16 writeNumberAsync(INT32 value)
{
    UINT8 ret = 0;
    char text[6];
    
    if(value < 0)
    {
       writeStringAsync("-");
       value = value * -1;
    }
    char temp5 = getHexValue((value % 100000) / 10000);
    char temp4 = getHexValue((value % 10000)  / 1000);
    char temp3 = getHexValue((value % 1000)   / 100);
    char temp2 = getHexValue(((value % 100)   / 10));
    char temp1 = getHexValue((value % 10));
    
    if(temp5 == 0)
    {
        if(temp4 == 0)
        {
            if(temp3 == 0)
            {
                if(temp2 == 0)
                {
                    text[0] = temp1;
                    text[1] = '\0';
                }
                else
                {
                    text[0] = temp2;
                    text[1] = temp1;
                    text[2] = '\0';
                }
            }
            else
            {
                text[0] = temp3;
                text[1] = temp2;
                text[2] = temp1;
                text[3] = '\0';
            }
        }
        else
        {
            text[0] = temp4;
            text[1] = temp3;
            text[2] = temp2;
            text[3] = temp1;
            text[4] = '\0';
        }
    }
    else
    {
        text[0] = temp5;
        text[1] = temp4;
        text[2] = temp3;
        text[3] = temp2;
        text[4] = temp1;
        text[5] = '\0';
    }

    writeStringAsync(text);
    
    return ret;

}

INT16 writeNumberBlk(INT32 value)
{
    UINT8 ret = 0;
    
    writeStringBlk("0x");

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

static INT16 writeChar(INT8 text)
{
    UINT8 ret = 0;
    while( ret == 0)
    {
        if (0 == U1STAbits.UTXBF) //0 = FIFO is not full
        {
            U1TXREG = text;// Transmit one char
            ret = 1;
        }
    }

    return ret;
}

static void writeCharAsync(INT8 text)
{
    INT8 tst = 0;
    *pRingHead = text;// Transmit one char
    pRingHead++;
    /* Wrap around the ring head */
    if( pRingHead > &ringBuff[RING_SIZE-1] )
        pRingHead = &ringBuff[0];
    
    /* Ring buffer overflow, force a math exception */
    if( pRingHead == pRingTail)
        text = text / tst; /* Divide by zero exception */
}