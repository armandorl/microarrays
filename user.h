/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define ADC_CHANNELS 4
#define BL_SQRT      7
#define BLOCKSIZE    128      /* BLOCKSIZE IS ALWAYS 2 POWER TO BL_SQRT */

        /* ENERGY_THRESHOLD =((1024(MAX ADC)^2)*64(BLOCKSIZE))/2 */
#define ENERGY_THRESHOLD    (float)10000
#define COUNTER_HI_LIMIT    10
#define COUNTER_HI_ACTIVE    7
#define COUNTER_LOW_INACTIVE    3
#define COUNTER_LOW_LIMIT    0
#define CHANNEL_ENERGY_ARRAY    10

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */
void InitDma(void);         /* DMA Initialization */
void InitSerial(void);      /* UART initialization */
float ProcessADCSamples(unsigned int *,unsigned int *, int, int);
void CalculateAverage(unsigned int *, int);
void delay(long);
void firFilter(float[],float[], float *);

typedef struct tagBufferA{
    unsigned int channel[ADC_CHANNELS][BLOCKSIZE];
} Buffer;

extern Buffer BufferA_regs __attribute__((aligned(128)));
extern Buffer BufferB_regs __attribute__((aligned(128)));
extern long counter[ADC_CHANNELS];
extern float filteredSignal[BLOCKSIZE];

extern unsigned int CHANNELA0_OFFSET;
extern unsigned int CHANNELA1_OFFSET;
extern unsigned int CHANNELA2_OFFSET;
extern unsigned int CHANNELA3_OFFSET;
