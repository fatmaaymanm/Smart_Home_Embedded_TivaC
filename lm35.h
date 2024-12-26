#include <stdint.h>
#include "tm4c123gh6pm.h"

#ifndef LM35_H
#define LM35_H

void LM35_Init(void);
uint32_t Get_Temperature(void);

#endif /* LM35H */