
#ifndef USERDEF_H
#define USERDEF_H

#include <dsp.h>
#include "fft.h"
/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define ADC_CHANNELS              2

        /* ENERGY_THRESHOLD =((1024(MAX ADC)^2)*64(BLOCKSIZE))/2 */
#define ENERGY_THRESHOLD          (float)10000
#define COUNTER_HI_LIMIT          10
#define COUNTER_HI_ACTIVE         7
#define COUNTER_LOW_INACTIVE      3
#define COUNTER_LOW_LIMIT         0
#define CHANNEL_ENERGY_ARRAY      10


#define FP  39875000
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

/* User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */
void InitDma(void);         /* DMA Initialization */
void InitSerial(void);      /* UART initialization */

void InitADCSignals(BufferType *inBuffer);
INT16 ProcessADCSamples(fractcomplex *signal1, fractcomplex *signal2);
FLOAT32 CalculateAverage(fractcomplex *signal);
void calibration(void);
void adcService(void);
void storeValues(void);

// Assembly imported operations
extern void ScaleSignal(fractcomplex * signal);
extern INT32 Calibrate(INT16 * signal);
extern void CalcTcy(void);

// User globals
extern INT8 currentFrame;
extern INT8 activeBuffer;
extern INT8 startService;
extern UINT8 StartFlag;

// Global offsets and gains used for calibration
extern UINT16 channel0_offset;
extern UINT16 channel1_offset;
extern UINT16 channel2_offset;
extern UINT16 channel3_offset;

extern UINT16 channel0_gains;
extern UINT16 channel1_gains;
extern UINT16 channel2_gains;
extern UINT16 channel3_gains;

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
extern fractional hammingWindow[FFT_BLOCK_LENGTH];

extern const INT16 FFT_BINS[FFT_BLOCK_LENGTH];

#endif
