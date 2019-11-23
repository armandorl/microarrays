/* 
 * File:   serialDriver.h
 * Author: arm-asus
 *
 * Created on 9 de junio de 2015, 10:44 PM
 */

#ifndef SERIALDRIVER_H
#define	SERIALDRIVER_H

#include "cdefs.h"

#define MAX_UART_CHARS 128

void InitRingBuffer(void);
void ringSendData(void);

INT16 writeStringAsync(INT8 * text);
INT16 writeNumberAsync(INT32 value);
INT16 writeRoundFloatNumAsync(FLOAT32 value);

INT16 writeStringBlk(INT8 * text);
INT16 writeNumberBlk(INT32 value);
INT16 writeRoundFloatNumBlk(FLOAT32 value);

INT8 getCharacter(void);

#endif	/* SERIALDRIVER_H */

