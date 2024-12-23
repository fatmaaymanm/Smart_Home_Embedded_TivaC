#ifndef DOOR_H
#define DOOR_H

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "DIO.h"

#define DOOR_OPEN  0
#define DOOR_CLOSED 1

void Door_Init(char port, uint8_t pin);
uint8_t Get_Door_Status(char port, uint8_t pin);
void Update_Door_OpenCount(char port, uint8_t pin);
uint32_t Get_Door_OpenCount(char port, uint8_t pin);

#endif


