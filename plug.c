#include "plug.h"

// Initialize the plug pin
void Plug_Init(char port, uint8_t pin) {
    // Configure the plug pin as output
    dio_init(port, pin, Output);
}

// Control the plug state (ON/OFF)
void Plug_Control(char port, uint8_t pin, uint8_t state) {
    if (state == PLUG_ON) {
        dio_writepin(port, pin, 1); // Activate the plug
    } else if (state == PLUG_OFF) {
        dio_writepin(port, pin, 0); // Deactivate the plug
    }
}

// Toggle the plug state
void Plug_Toggle(char port, uint8_t pin) {
    uint8_t current_state = dio_readpin(port, pin);
    dio_writepin(port, pin, !current_state); // Toggle the plug state
}