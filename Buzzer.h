#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "DIO.h"

//#define High_temp 10.0
#define BUZZER_PIN 0


void Buzzer_init();
void Buzzer_activate();
void Buzzer_deactivate();


#endif 