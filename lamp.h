#ifndef LAMP_H
#define LAMP_H

#include <stdint.h>
#include "DIO.h"

// LED state definitions
#define LAMP_ON  0
#define LAMP_OFF 1

// Function prototypes
void Lamp_Init(char port, uint8_t pin);
void Lamp_On(char port, uint8_t pin);
void Lamp_Off(char port, uint8_t pin);
void Lamp_Toggle(char port, uint8_t pin);
uint8_t Get_Lamp_State(char port, uint8_t pin);

#endif // LED_H
