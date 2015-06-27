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
    RPINR18bits.U1RXR = 0x27; /* 010 0111 RP39/RB7 to UART1 Rx */
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
    AD1CON2bits.CHPS = 2; // Multichannel 4 channels
//    AD1CON2bits.CHPS = 1; // Only CH0 and CH1
    AD1CON2bits.VCFG = 0;  // Vrefh = AVdd VrefL = AVss
    AD1CON2bits.CSCNA = 0; // Do not scan multiple inputs

    AD1CON2bits.BUFM = 0; // Fill buffer from ADC0
    AD1CON2bits.SMPI = 0; // Interrupt after 2 inputs acquired

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
    AD1CON3bits.ADCS = 15; // Freq / 3
    // TAD = TCY * (ADCS + 1)
    // TAD = 25.2438 * 1 =  25.2438 ns
    // TCONV = 12 * TAD = 25.2438 ns * 12 = 908.7 ns (1.1 Mhz)
    // ADCS = 164 = Tconv -> 50us

    // TSIM = TSMP + (M ? TCONV)

    // Sample time
    AD1CON3bits.SAMC = 10;

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
    
    // Enable simultaneous sampling
    AD1CON1bits.SIMSAM = 1;

    // DMA Usage
    AD1CON4bits.ADDMAEN = 0; // DMA Disabled

    //IFS0bits.AD1IF = 1;
    IEC0bits.AD1IE = 1;
    IPC3bits.AD1IP = 5; // Higher priority for ADC interrupts

    INTCON2bits.GIE = 1; // Enable global interrupts
    
}
