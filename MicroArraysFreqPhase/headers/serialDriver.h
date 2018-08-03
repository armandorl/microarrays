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

INT16 writeString(INT8 * text);
INT16 writeNumber(INT32 value);
INT16 writeRoundFloatNum(FLOAT32 value);
INT8 getCharacter(void);

#endif	/* SERIALDRIVER_H */

