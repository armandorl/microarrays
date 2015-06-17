
#ifndef USERDEF_H
#define USERDEF_H

#include <dsp.h>
#include "fft.h"
/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define ADC_CHANNELS              2
#define BL_SQRT                   8
#define BLOCKSIZE                 256

        /* ENERGY_THRESHOLD =((1024(MAX ADC)^2)*64(BLOCKSIZE))/2 */
#define ENERGY_THRESHOLD          (float)10000
#define COUNTER_HI_LIMIT          10
#define COUNTER_HI_ACTIVE         7
#define COUNTER_LOW_INACTIVE      3
#define COUNTER_LOW_LIMIT         0
#define CHANNEL_ENERGY_ARRAY      10


#define FP 39875000
#define BAUDRATE 115200
#define BRGVAL ((FP/BAUDRATE)/16)-1
#define DELAY_105uS asm volatile ("REPEAT, #201"); Nop();// 105uS dela

/* Application specific user parameters used in user.c may go here */

typedef char INT8;
typedef int  INT16;
typedef long INT32;

typedef unsigned char UINT8;
typedef unsigned int  UINT16;
typedef unsigned long UINT32;

typedef float FLOAT32;
typedef long double FLOAT64;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */
void InitDma(void);         /* DMA Initialization */
void InitSerial(void);      /* UART initialization */

INT16 ProcessADCSamples(INT16 *signal, UINT8 channel);
void CalculateAverage(INT16 *signal, UINT8 channel);
INT8 calibration(void);
void adcService(void);
void storeValues(void);

// Assembly imported operations
extern void ScaleSignal(INT16 * signal);
extern INT32 Calibrate(INT16 * signal);
extern void CalcTcy(void);
extern void SquareMagnitude(INT16 size, INT16* srcArray, INT16* dstArray);

extern INT8 activeBuffer;
extern INT8 startService;
extern UINT8 StartFlag;
extern INT16 BufferA_regs[ADC_CHANNELS][BLOCKSIZE];
extern INT16 BufferB_regs[ADC_CHANNELS][BLOCKSIZE];
extern const fractcomplex twiddleFactors[FFT_BLOCK_LENGTH/2]	/* Twiddle Factor array in Program memory */
__attribute__ ((space(auto_psv), aligned (FFT_BLOCK_LENGTH*2)));


#endif
