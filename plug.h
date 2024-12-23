#ifndef PLUG_H
#define PLUG_H

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "DIO.h"

// Plug States
#define PLUG_ON  1
#define PLUG_OFF 0

// Function Prototypes
void Plug_Init(char port, uint8_t pin);
void Plug_Control(char port, uint8_t pin, uint8_t state);

#endif // PLUG_H