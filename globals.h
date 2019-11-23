
#ifndef USERDEF_H
#define USERDEF_H

#include <dsp.h>
#include "cdefs.h"
#include "fft.h"
#include "dsp.h"

/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define HANNING_WINDOW

/* ADC Input buffers */
typedef struct
{
    fractional Adc1Ch0[FFT_BLOCK_LENGTH];
    fractional Adc1Ch1[FFT_BLOCK_LENGTH];
    fractional Adc1Ch2[FFT_BLOCK_LENGTH];
    fractional Adc1Ch3[FFT_BLOCK_LENGTH];
} BufferType;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/


/******************************************************************************/
/* User Global Variables                                                      */
/******************************************************************************/

// User globals
extern BufferType BufferA;
extern BufferType BufferB;

extern fractcomplex Buffer0_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far));
extern fractcomplex Buffer1_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far));
extern fractcomplex Buffer2_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far));
extern fractcomplex Buffer3_regs[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far));

extern fractcomplex Buffer_results1[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far));
extern fractcomplex Buffer_results2[FFT_BLOCK_LENGTH] __attribute__((space(ymemory), aligned(FFT_BLOCK_LENGTH * 2 *2), far));
/* Twiddle Factor array in Program memory */
extern fractcomplex twiddleFactors[FFT_BLOCK_LENGTH/2] 	/* Declare Twiddle Factor array in X-space*/
__attribute__ ((section (".xbss, bss, xmemory"), aligned (FFT_BLOCK_LENGTH*2)));
extern fractional hanningWindow[FFT_BLOCK_LENGTH];


#endif
