#include "Buzzer.h"


#define SYSCTL_RCGCGPIO   (*(volatile uint32_t *)0x400FE608)
#define SYSCTL_PRGPIO     (*(volatile uint32_t *)0x400FEA08)
#define GPIO_PORTB_DATA_R (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R  (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_DEN_R  (*((volatile unsigned long *)0x4000551C))

#define SYSCTL_RCGCGPIO_R1 0x00000002  // Port B Clock Gating Control


void Buzzer_init(){
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; 
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0) {}
    
    GPIO_PORTB_DIR_R |= (1 << BUZZER_PIN);
    GPIO_PORTB_DEN_R |= (1 << BUZZER_PIN); 

}
void Buzzer_activate(){
    GPIO_PORTB_DATA_R |= (1 << BUZZER_PIN);
}
void Buzzer_deactivate(){
    GPIO_PORTB_DATA_R &= ~(1 << BUZZER_PIN);
}