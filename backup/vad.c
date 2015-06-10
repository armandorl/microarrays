/* 
 * File:   vad.c
 * Author: arm-asus
 *
 * Created on 22 de septiembre de 2013, 01:09 PM
 */
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
/*
 * 
 */
long counter[ADC_CHANNELS]={0,0,0,0};

void ProcessADCSamples(fractional *buf, int offset, int channel){
    int i=0;
    fractional energy_sum = 0;
    long zero_crossing = 0;
    fractional prev_val;
    fractional correctedSignal[BLOCKSIZE];
//    fractional filteredSignal[BLOCKSIZE];
//    fractcomplex corr4fft[BLOCKSIZE];
//    fractcomplex fftSignal[BLOCKSIZE];

    //firFilter(buf, &filteredSignal[0]);

    // Features extraction
    for( ; i < BLOCKSIZE; i++){
        fractional a = (fractional)*(buf+i);
        correctedSignal[i] = a - offset;
//        corr4fft[0].real = correctedSignal[i];
//        corr4fft[0].imag = 0;
        energy_sum += (correctedSignal[i]*correctedSignal[i]);
        if(i > 0){
            if( correctedSignal[i] > 0 && prev_val < 0  ){
                zero_crossing++;
            }else if( correctedSignal[i] < 0 && prev_val > 0  ){
                zero_crossing++;
            }
            
        }
        prev_val=correctedSignal[i];
    }

    //FFTComplex(log2N,&fftSignal, &corr4fft, twidFactors, factPage);


    // VAD Algorithm
    if(energy_sum > ENERGY_THRESHOLD){
        PORTBbits.RB14 = 1;
    }else{
        PORTBbits.RB14 = 0;
    }
   

}

