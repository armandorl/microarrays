/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
#ifndef XC_SYSTEM_H
#define	XC_SYSTEM_H
/* Define system operating frequency */

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        115500000L
#define FCY             SYS_FREQ/2

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */

#endif
