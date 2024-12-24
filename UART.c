#include "UART.h"

void UART0_Init(void) {
  SYSCTL_RCGCUART_R |= (1 << 0);
  SYSCTL_RCGCGPIO_R |= (1 << 0);
  while ((SYSCTL_PRUART_R & (1 << 0)) == 0);
  while ((SYSCTL_PRGPIO_R & (1 << 0)) == 0);
  
  GPIO_PORTA_AFSEL_R |= (1 << 0) | (1 << 1);
  GPIO_PORTA_PCTL_R &= ~0xFF;
  GPIO_PORTA_PCTL_R |= (1 << 0) | (1 << 4);
  GPIO_PORTA_DEN_R |= (1 << 0) | (1 << 1);
  GPIO_PORTA_AMSEL_R &= ~((1 << 0) | (1 << 1));
  
  UART0_CTL_R &= ~(1 << 0);
  UART0_IBRD_R = 104;
  UART0_FBRD_R = 11;
  UART0_LCRH_R = (0x3 << 5); // 8-bit word length, no parity, 1 stop bit
  UART0_CC_R = 0x0; ///////////NOTE/////////////////////////////////////
  UART0_CTL_R |= (1 << 0) | (1 << 8) | (1 << 9);  // Enable UART0, Tx, Rx
}

void UART0_TransmitChar(char c) {
  while ((UART0_FR_R & (1 << 5)) != 0);  // Wait until the transmit FIFO is not full
  UART0_DR_R = c;  // Write the character to the data register
}

char UART0_ReceiveChar(void) {
  while ((UART0_FR_R & (1 << 4)) != 0);  // Wait until the receive FIFO is not empty
  return (char)(UART0_DR_R & 0xFF);  // Read and return the received character
}

