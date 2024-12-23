#include "Relay.h"

// Initialize the relay pins
void Relay_Init(char port, uint8_t pin) {
    // Configure the relay pins as output
    dio_init(port, pin, Output);
}

// Control the relay state (ON/OFF)
void Relay_Control(char port, uint8_t pin, uint8_t state) {
    if (state == RELAY_ON) {
        dio_writepin(port, pin, 1); // Activate the relay
    } else if (state == RELAY_OFF) {
        dio_writepin(port, pin, 0); // Deactivate the relay
    }
}

// Toggle the relay state
void Relay_Toggle(char port, uint8_t pin) {
    uint8_t current_state = dio_readpin(port, pin);
    dio_writepin(port, pin, !current_state); // Toggle the relay state
}
