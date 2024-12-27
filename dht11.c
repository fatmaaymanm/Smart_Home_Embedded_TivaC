#include "DHT11.h"

// Delay function
static void DelayUs(uint32_t us) {
    while (us--) {
        for (uint32_t i = 0; i < 16; i++) {
            __asm("NOP");
        }
    }
}

// Initialize PC4 for DHT11 communication
void DHT11_Init(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2; // Enable clock for PORTC
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R2) == 0); // Wait for clock to stabilize

    GPIO_PORTC_DIR_R |= DHT11_PIN_MASK; // Set PC4 as output
    GPIO_PORTC_DEN_R |= DHT11_PIN_MASK; // Enable digital function
    GPIO_PORTC_DATA_R |= DHT11_PIN_MASK; // Set PC4 high
}

// Read temperature from DHT11
uint8_t DHT11_ReadData(void) {
    uint8_t data[5] = {0};

    // Send start signal
    GPIO_PORTC_DIR_R |= DHT11_PIN_MASK; // Set as output
    GPIO_PORTC_DATA_R &= ~DHT11_PIN_MASK; // Pull pin low
    DelayUs(18000); // 18ms
    GPIO_PORTC_DATA_R |= DHT11_PIN_MASK; // Pull pin high
    DelayUs(20); // 20us

    // Wait for response
    GPIO_PORTC_DIR_R &= ~DHT11_PIN_MASK; // Set as input
    DelayUs(40);
    if ((DHT11_PORT & DHT11_PIN_MASK) != 0) return 0; // No response
    DelayUs(80);
    if ((DHT11_PORT & DHT11_PIN_MASK) == 0) return 0; // Invalid response

    DelayUs(80);

    // Read 40 bits of data
    for (uint8_t i = 0; i < 5; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            while ((DHT11_PORT & DHT11_PIN_MASK) == 0); // Wait for low-to-high transition
            DelayUs(40); // Measure the duration of high level
            if ((DHT11_PORT & DHT11_PIN_MASK) != 0) {
                data[i] |= (1 << (7 - j)); // If high for more than 40us, it's a 1
            }
            while ((DHT11_PORT & DHT11_PIN_MASK) != 0); // Wait for high-to-low transition
        }
    }

    // Verify checksum
    if (data[4] != (data[0] + data[1] + data[2] + data[3])) {
        return 0; // Checksum error
    }
     // Return the temperature value
    return data[2]; // Temperature in Celsius
}