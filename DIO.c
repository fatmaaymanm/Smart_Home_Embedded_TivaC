#include "DIO.h"

// Function to initialize a port and pins
void dio_init(char port, uint8_t pin, uint8_t direction) {
    uint32_t port_base = 0;

    // Select base address based on port letter
    switch (port) {
        case 'A': port_base = GPIO_PORTA_BASE; break;
        case 'B': port_base = GPIO_PORTB_BASE; break;
        case 'C': port_base = GPIO_PORTC_BASE; break;
        case 'D': port_base = GPIO_PORTD_BASE; break;
        case 'E': port_base = GPIO_PORTE_BASE; break;
        case 'F': port_base = GPIO_PORTF_BASE; break;
        default: return;  // Invalid port
    }

    // Enable the clock for the port
    SET_BIT(SYSCTL_RCGCGPIO_R, (port - 'A'));
    while (GET_BIT(SYSCTL_PRGPIO_R, (port - 'A')) == 0);

    // Unlock mechanism for locked pin
    if (port == 'F' && (pin & Pin0)) {  // Check if PF0 is part of the pins
        GPIO_PORT_LOCK_R(port_base) = 0x4C4F434B;  // Unlock the port
        GPIO_PORT_CR_R(port_base) |= pin;        // Commit register for the pins
    }

    // Configure the pins as input or output
    if (direction == Output) {
        SET_BIT(GPIO_PORT_DIR_R(port_base), pin);  // Set pin as output
    } else if (direction == Input) {
        CLEAR_BIT(GPIO_PORT_DIR_R(port_base), pin);  // Set pin as input
        // Enable pull-up resistors for the input pins
        SET_BIT(GPIO_PORT_PUR_R(port_base), pin);
    }

    // Enable the digital functionality for the pins
    SET_BIT(GPIO_PORT_DEN_R(port_base), pin);
}

// Function to read a specific pin
uint8_t dio_readpin(char port, uint8_t pin) {
    uint32_t port_base = 0;

    // Select base address based on port letter
    switch (port) {
        case 'A': port_base = GPIO_PORTA_BASE; break;
        case 'B': port_base = GPIO_PORTB_BASE; break;
        case 'C': port_base = GPIO_PORTC_BASE; break;
        case 'D': port_base = GPIO_PORTD_BASE; break;
        case 'E': port_base = GPIO_PORTE_BASE; break;
        case 'F': port_base = GPIO_PORTF_BASE; break;
        default: return 0;  // Invalid port check
    }

    // Read the pin's value
    return GET_BIT(GPIO_PORT_DATA_R(port_base), pin);
}

// Function to read the entire port
uint8_t dio_readport(char port) {
    uint32_t port_base = 0;

    // Select base address based on port letter
    switch (port) {
        case 'A': port_base = GPIO_PORTA_BASE; break;
        case 'B': port_base = GPIO_PORTB_BASE; break;
        case 'C': port_base = GPIO_PORTC_BASE; break;
        case 'D': port_base = GPIO_PORTD_BASE; break;
        case 'E': port_base = GPIO_PORTE_BASE; break;
        case 'F': port_base = GPIO_PORTF_BASE; break;
        default: return 0;  // Invalid port check
    }

    // Read the entire port's value
    return (uint8_t)(*(volatile uint32_t *)(port_base + 0x3FC));
}

// Function to write to a specific pin
void dio_writepin(char port, uint8_t pin, uint8_t value) {
    uint32_t port_base = 0;

    // Select base address based on port letter
    switch (port) {
        case 'A': port_base = GPIO_PORTA_BASE; break;
        case 'B': port_base = GPIO_PORTB_BASE; break;
        case 'C': port_base = GPIO_PORTC_BASE; break;
        case 'D': port_base = GPIO_PORTD_BASE; break;
        case 'E': port_base = GPIO_PORTE_BASE; break;
        case 'F': port_base = GPIO_PORTF_BASE; break;
        default: return;  // Invalid port check
    }

    // Write value to the pin
    if (value) {
        SET_BIT(GPIO_PORT_DATA_R(port_base), pin);
    } else {
        CLEAR_BIT(GPIO_PORT_DATA_R(port_base), pin);
    }
}


// Function to write to the entire port
void dio_writeport(char port, uint8_t value) {
    uint32_t port_base = 0;

    // Select base address based on port letter
    switch (port) {
        case 'A': port_base = GPIO_PORTA_BASE; break;
        case 'B': port_base = GPIO_PORTB_BASE; break;
        case 'C': port_base = GPIO_PORTC_BASE; break;
        case 'D': port_base = GPIO_PORTD_BASE; break;
        case 'E': port_base = GPIO_PORTE_BASE; break;
        case 'F': port_base = GPIO_PORTF_BASE; break;
        default: return;  // Invalid port check
    }

    // Write the entire port's value
    GPIO_PORT_DATA_R(port_base) = value;
}
