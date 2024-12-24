
#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>
void SysTick_Init(uint32_t);
uint32_t SysTick_Is_Time_Out(void);
void LED_Delay(uint32_t);


// Initialize PORTC pin for DHT11
void DHT11_Init(void);

// Start signal to DHT11
void DHT11_StartSignal(void);

// Read temperature from DHT11 (integer part only)
uint8_t DHT11_ReadTemperature(void);

#endif
