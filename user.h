
#ifndef USERDEF_H
#define USERDEF_H

/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define ADC_CHANNELS              4
#define BL_SQRT                   6
#define BLOCKSIZE                 64      /* BLOCKSIZE IS ALWAYS 2 POWER TO BL_SQRT */
#define MAX_ADC_VALUE             1024

        /* ENERGY_THRESHOLD =((1024(MAX ADC)^2)*64(BLOCKSIZE))/2 */
#define ENERGY_THRESHOLD          (float)10000
#define COUNTER_HI_LIMIT          10
#define COUNTER_HI_ACTIVE         7
#define COUNTER_LOW_INACTIVE      3
#define COUNTER_LOW_LIMIT         0
#define CHANNEL_ENERGY_ARRAY      10

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
void adcService(void);

// Assembly imported operations
extern void ScaleSignal(INT16 * signal, INT16 offset);
extern INT16 Calibrate(INT16 * signal);

typedef struct tagBufferA{
    INT16 channel[ADC_CHANNELS][BLOCKSIZE];
} Buffer;

extern Buffer BufferA_regs;
extern Buffer BufferB_regs;

extern INT16 _PERSISTENT CHANNEL_OFFSET[ADC_CHANNELS];
extern INT16 _PERSISTENT CHANNEL_GAIN[ADC_CHANNELS];
extern UINT8 _PERSISTENT CALIBRATION_AVAILABLE;

#endif
