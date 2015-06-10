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
#include "user.h"            /* variables/params used by user.c               */

//#define FP 40000000
//#define FP 68750000
#define FP 39613687
#define BAUDRATE 115200
#define BRGVAL ((FP/BAUDRATE)/16)-1
#define DELAY_105uS asm volatile ("REPEAT, #201"); Nop();// 105uS dela

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitSerial(void)
{

    /* Set as Digital Inputs/Outputs */
    TRISBbits.TRISB7 = 1;
    TRISBbits.TRISB8 = 0;
    ANSELBbits.ANSB8 = 0;

    /* Port mapping */
    RPINR38bits.DTCMP1R = 0x27;  /* 010 0111 RP39/RB7 to UART1 Rx */
    RPOR3bits.RP40R = 1; /* RP40/RB8 tied to UART1 Transmit */

    // Configure oscillator as needed
    U1MODEbits.STSEL = 0;// 1-Stop bit
    U1MODEbits.PDSEL = 1;// eVEN Parity, 8-Data bits
    U1MODEbits.ABAUD = 0;// Auto-Baud disabled
    U1MODEbits.BRGH = 0;// Standard-Speed mode
    U1BRG = BRGVAL;// Baud Rate
    U1STAbits.UTXISEL0 = 0;// Interrupt after one TX character is transmitted
    U1STAbits.UTXISEL1 = 0;
//    U1STAbits.URXISEL = 0;// Interrupt after one RX character is received;
//    IEC0bits.U1TXIE = 1;// Enable UART TX interrupt
    IEC0bits.U1TXIE = 0;// Diasble UART TX interrupt
//    IEC0bits.U1RXIE = 1;// Enable UART RX interrupt
    U1MODEbits.UARTEN = 1;// Enable UART
    U1STAbits.UTXEN = 1;// Enable UART TX


    /* Wait at least 105 microseconds (1/9600) before sending first char */
    DELAY_105uS
    U1TXREG = 'a';// Transmit one characterwhile(1){}

}


void InitApp(void)
{
    /* TODO Initialize User Ports/Peripherals/Project here */
    TRISBbits.TRISB14 = 0;
    TRISBbits.TRISB15 = 0;

    /* Setup analog functionality and port direction */
    AD1CON1bits.AD12B = 0; // 10-bit, 4 channel ADC
    AD1CON1bits.FORM = 0; // Form 0000 dddd dddd dddd Integer
    AD1CON2bits.CHPS = 2; // Multichannel
    AD1CON2bits.VCFG = 0;  // Vrefh = AVdd VrefL = AVss
    AD1CON2bits.CSCNA = 0; // Do not scan multiple inputs

    // Channel selection with Vrefl = AVss
    /*
     * CH0 = AN3
     * CH1 = AN0
     * CH2 = AN1
     * CH3 = AN2
     * 
     * */
    AD1CHS123bits.CH123NA = 0;
    AD1CHS123bits.CH123SA = 0;
    AD1CHS0bits.CH0NA = 0;
    AD1CHS0bits.CH0SA = 3;

    // ADC Tad Time
    // TCY = 1/FCY = 25.2438 ns
//    AD1CON3bits.ADRC = 0; // Internal timer off
    AD1CON3bits.ADRC = 0; // System clock
    //AD1CON3bits.ADCS = 2; // Freq / 3
    AD1CON3bits.ADCS = 161; // Freq / 3
    // TAD = TCY * (ADCS + 1)
    // TAD = 25.2438 * 3 =  75.7 ns
    // Tconv = 12 * TAD = 75.7 ns * 12 = 908.7 ns (1.1 Mhz)
    // ADCS = 164 = Tconv -> 50us

    // Sample time -- not used if timer 3 in use
    //AD1CON3bits.SAMC = 31; // 2.34 us

    // Set as Inputs
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    
    // Set as Analogue Inputs
    ANSELAbits.ANSA0 = 1;
    ANSELAbits.ANSA1 = 1;
    ANSELBbits.ANSB0 = 1;
    ANSELBbits.ANSB1 = 1;
    
    // Control ADC
    AD1CON1bits.ASAM = 1; // Automatic sampling on
    AD1CON1bits.SSRCG = 0; // Automatic
    AD1CON1bits.SSRC = 7; // Automatic conversion
    
//    AD1CON1bits.SSRCG = 0; // Trigger by timer 3
//    AD1CON1bits.SSRC = 2; // Trigger by timer 3
//    AD1CON1bits.SSRC = 0; // Manual sampling
//    TMR3 = 0x0000;
//    PR3 = 1980;// Trigger ADC1 every 50us (TCY * PR3)
    //TEST
    //PR3 = 50;// Trigger ADC1 every 5usec (TCY * PR3)
    IFS0bits.T3IF = 0;// Clear Timer3 interrupt
    IEC0bits.T3IE = 0;// Disable Timer3 interrupt
    
    // Enable simultaneous sampling
    AD1CON1bits.SIMSAM = 1;

    // DMA Usage
    InitDma();

    //IFS0bits.AD1IF = 1;
    IEC0bits.AD1IE = 1;
    IPC3bits.AD1IP = 5; // Higher priority for ADC interrupts

    INTCON2bits.GIE = 1; // Enable global interrupts
    


    /* Initialize peripherals */

}


void InitDma(void){

    DMA0STAL = (unsigned int)&BufferA_regs;
    DMA0STAH = 0x0000;
    DMA0STBL = (unsigned int)&BufferB_regs;
    DMA0STBH = 0x0000;
    
    DMA0CONbits.AMODE = 2; // Configure DMA for Peripheral Indirect Addressing
    DMA0CONbits.SIZE = 0; // Word Size
    DMA0CONbits.DIR = 0; //Read from peripheral
    AD1CON1bits.ADDMABM = 0; // DMA buffers are built in scatter/gather mode

    //Increments the DMA address after completion of every N+1 sample/conversion operation
    AD1CON2bits.SMPI = 3; // Every 4 sample/conversion DMA address increases
    AD1CON4bits.DMABL = BL_SQRT;  //2^6 words for each channel
    AD1CON4bits.ADDMAEN = 1; // DMA Enabled

    DMA0CONbits.HALF = 0; // Interruption at half CNT Disabled
    //DMA0CONbits.HALF = 1; // Interruption at half Enabled
    DMA0CONbits.MODE = 2; // Configure DMA for Continuous Ping-Pong mode
    DMA0PAD = (volatile unsigned int)&ADC1BUF0; // Point DMA to ADC1BUF0
    DMA0CNTbits.CNT = 4*(BLOCKSIZE * ADC_CHANNELS)-1; // 64 DMA request
    DMA0REQ = 13; // Select ADC1 as DMA Request source

    IFS0bits.DMA0IF = 0; // Clear the DMA Interrupt Flag bit
    IEC0bits.DMA0IE = 1; // Set the DMA Interrupt Enable bit
    DMA0CONbits.CHEN = 1; // Enable DMA
}

void delay(long number){
    int i=0;
    for( ; i<number;i++){
        asm("NOP");
    }
}