#include "lm35.h"

void LM35_Init(void) {
      // Enable clocks
    SYSCTL_RCGCGPIO_R |= 0x20;      // enable clock to GPIO_PORTF (for LED)
    SYSCTL_RCGCGPIO_R |= 0x10;      // enable clock to GPIO_PORTE (for ADC)
    SYSCTL_RCGCADC_R |= 1;          // enable clock to ADC0
    
    // Initialize PE3 for AIN0 input (LM35 sensor input)
    GPIO_PORTE_AFSEL_R |= 8;        // enable alternate function
    GPIO_PORTE_DEN_R &= ~8;         // disable digital function
    GPIO_PORTE_AMSEL_R |= 8;        // enable analog function
    
    // Initialize PF1 (Red LED)
    GPIO_PORTF_DIR_R |= 0x02;      // Set PF1 as output (Red LED)
    GPIO_PORTF_DEN_R |= 0x02;      // Enable digital function for PF1

    // Initialize ADC0
    ADC0_ACTSS_R &= ~8;             // disable SS3 during configuration
    ADC0_EMUX_R &= ~0xF000;         // software trigger conversion
    ADC0_SSMUX3_R = 0;              // get input from channel 0 (AIN0)
    ADC0_SSCTL3_R |= 6;             // take one sample at a time, set flag at 1st sample
    ADC0_ACTSS_R |= 8;              // enable ADC0 sequencer 3
}

uint32_t Get_Temperature(void) {
  int temperature;
  ADC0_PSSI_R |= 8;           // start a conversion sequence 3
  while((ADC0_RIS_R & 0x08) == 0)
      ;                       // wait for conversion to complete
  
  // Calculate temperature in Celsius
  temperature = ((ADC0_SSFIFO3_R * 330) / 4096);
  
  ADC0_ISC_R = 8;             // clear completion flag
  
  return temperature;
}