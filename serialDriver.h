/* 
 * File:   serialDriver.h
 * Author: arm-asus
 *
 * Created on 9 de junio de 2015, 10:44 PM
 */

#ifndef SERIALDRIVER_H
#define	SERIALDRIVER_H

#define MAX_UART_CHARS 128

int writeString(char * text);
int writeNumber(int value);

#endif	/* SERIALDRIVER_H */

