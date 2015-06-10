
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dsp.h>


/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define ADC_CHANNELS 4
#define BL_SQRT      6
#define BLOCKSIZE    64      /* BLOCKSIZE IS ALWAYS 2 POWER TO BL_SQRT */
#define CHANNELA0_OFFSET    493
#define CHANNELA1_OFFSET    493
#define CHANNELA2_OFFSET    493
#define CHANNELA3_OFFSET    493

        /* ENERGY_THRESHOLD =((1024(MAX ADC)^2)*64(BLOCKSIZE))/2 */
#define ENERGY_THRESHOLD    (float)1500000
#define COUNTER_HI_LIMIT    10
#define COUNTER_HI_ACTIVE    7
#define COUNTER_LOW_INACTIVE    3
#define COUNTER_LOW_LIMIT    0

/* Filter parameters*/
//#define NY 20


/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */
void InitDma(void);         /* DMA Initialization */
void ProcessADCSamples(fractional *, int, int);
//void firFilter(fractional *, fractional *);


typedef struct tagBufferA{
    fractional channel[ADC_CHANNELS][BLOCKSIZE];
} Buffer;

extern Buffer BufferA_regs __attribute__((address(0x1200)));
extern Buffer BufferB_regs __attribute__((address(0x1600)));
//extern fractional z[NY] __attribute__ ((space(ymemory),far));
extern long counter[ADC_CHANNELS];
//extern FIRStruct FIRfilter;




