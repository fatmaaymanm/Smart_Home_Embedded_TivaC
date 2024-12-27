#include "lamp.h"

// Initialize Lamp
void Lamp_Init(char port, uint8_t pin) {
    dio_init(port, pin, Output); // Configure pins as output
}

// Turn on Lamp
void Lamp_On(char port, uint8_t pin) {
    dio_writepin(port, pin, 1); // Set bit corresponding to Lamp
}

// Turn off Lamp
void Lamp_Off(char port, uint8_t pin) {
    dio_writepin(port, pin, 0); // Clear bit corresponding to Lamp
}

// Toggle Lamp
void Lamp_Toggle(char port, uint8_t pin) {
    dio_writeport(port, dio_readport(port) ^ (1 << pin)); // XOR to toggle bits
}

uint8_t Get_Lamp_State(char port, uint8_t pin) {
    return dio_readpin(port, pin);
}

