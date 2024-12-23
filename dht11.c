#include "dht11.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define DHT11_PIN (1U << 4) // Pin 4 on PORTC

// Blocking delay function using SysTick timer
void SysTick_Init(uint32_t ticks) {
    NVIC_ST_CTRL_R = 0;               // Disable SysTick during configuration
    NVIC_ST_RELOAD_R = ticks;         // Load the number of ticks for the delay
    NVIC_ST_CURRENT_R = 0;            // Clear the current value register
    NVIC_ST_CTRL_R = 0x05;            // Enable SysTick with system clock (no interrupts)
}

uint32_t SysTick_Is_Time_Out(void) {
    return (NVIC_ST_CTRL_R & 0x10000); // Check the COUNTFLAG in the CTRL register
}

void LED_Delay(uint32_t delayMs) {
    uint32_t ticks = delayMs * 16000; // Assuming a 16 MHz system clock
    SysTick_Init(ticks - 1);          // Initialize SysTick for the required delay
    while (!SysTick_Is_Time_Out());   // Wait for timeout
}

// Function to initialize PORTC pin
void DHT11_Init(void) {
    SYSCTL_RCGCGPIO_R |= (1U << 2); // Enable clock for PORTC
    while ((SYSCTL_PRGPIO_R & (1U << 2)) == 0); // Wait for clock stabilization

    GPIO_PORTC_DEN_R |= DHT11_PIN; // Enable digital functionality for PC4
    GPIO_PORTC_AFSEL_R &= ~DHT11_PIN; // Disable alternate function
    GPIO_PORTC_AMSEL_R &= ~DHT11_PIN; // Disable analog mode
}

// Function to send the start signal to the DHT11
void DHT11_StartSignal(void) {
    GPIO_PORTC_DIR_R |= DHT11_PIN;   // Set PC4 as OUTPUT
    GPIO_PORTC_DATA_R &= ~DHT11_PIN; // Pull pin LOW for 18ms
    LED_Delay(18);                   // Delay 18ms
    GPIO_PORTC_DATA_R |= DHT11_PIN;  // Pull pin HIGH for 20-30탎
    LED_Delay(1);                    // Delay 20-30탎
}

// Function to set the pin to INPUT mode
static void DHT11_SetPinInput(void) {
    GPIO_PORTC_DIR_R &= ~DHT11_PIN; // Set PC4 as INPUT
}

// Function to read a single bit from the DHT11
static uint8_t DHT11_ReadBit(void) {
    uint32_t timeCount = 0;

    while ((GPIO_PORTC_DATA_R & DHT11_PIN) == 0); // Wait for pin to go HIGH (50탎 LOW)
    LED_Delay(1); // Small delay to avoid false reads

    // Measure HIGH time to determine 0 or 1
    timeCount = 0;
    while (GPIO_PORTC_DATA_R & DHT11_PIN) { // While pin is HIGH
        timeCount++;
        LED_Delay(1); // Increment time with ~1탎 resolution
    }

    if (timeCount > 30) { // If HIGH duration > ~30탎, it's a '1'
        return 1;
    } else { // Else, it's a '0'
        return 0;
    }
}

// Function to read the entire data from DHT11
float DHT11_ReadTemperature(void) {
    uint8_t i, j;
    uint8_t data[5] = {0}; // Array to store 5 bytes from DHT11

    DHT11_StartSignal();   // Send start signal to DHT11
    DHT11_SetPinInput();   // Switch pin to INPUT to read sensor response

    // Wait for sensor's response signal
    while ((GPIO_PORTC_DATA_R & DHT11_PIN) != 0); // Wait for LOW
    while ((GPIO_PORTC_DATA_R & DHT11_PIN) == 0); // Wait for HIGH
    while ((GPIO_PORTC_DATA_R & DHT11_PIN) != 0); // Wait for LOW (end of response)

    // Read 40 bits (5 bytes) from DHT11
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
            data[i] <<= 1; // Shift left to make room for the next bit
            data[i] |= DHT11_ReadBit(); // Read a bit and store it
        }
    }

    // Checksum validation
    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return -1.0; // Return -1.0 to indicate an error
    }

    // Combine integer and decimal parts into a float value
    float temperature = data[2] + (data[3] / 10.0); // Integer part + Decimal part scaled
    return temperature;
}