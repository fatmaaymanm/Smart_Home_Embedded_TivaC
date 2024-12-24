#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "tm4c123gh6pm.h"

void UART0_Init(void);
void UART0_TransmitChar(char c);
char UART0_ReceiveChar(void);

#endif