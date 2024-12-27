#ifndef DHT11_H
#define DHT11_H

#include "tm4c123gh6pm.h"
#include <stdint.h>

// Define macros for the DHT11 pin
#define DHT11_PORT GPIO_PORTC_DATA_R
#define DHT11_PIN 4
#define DHT11_PIN_MASK (1U << DHT11_PIN)

// Function prototypes
void DHT11_Init(void);
uint8_t DHT11_ReadData(void);

#endif // DHT11_H