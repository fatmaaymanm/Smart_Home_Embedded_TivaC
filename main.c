#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "Buzzer.h"
#include "lamp.h"
#include "door.h"
#include "plug.h"
#include "lm35.h"

#define UART_BASE UART0_BASE

// Simulated temperature
volatile uint32_t temperature = 25;

// Function to initialize UART
void UART_Init(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0) || !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

// Function to send a string via UART
void UART_SendString(const char *str) {
    while (*str) {
        UARTCharPut(UART_BASE, *str++);
    }
}

void UART_SendNumber(int number) {
    char buffer[16];
    int index = 0;

    // Handle negative numbers
    if (number < 0) {
        buffer[index++] = '-';
        number = -number;
    }

    // Convert number to string (manual approach)
    int temp = number;
    int digits = 0;
    do {
        digits++;
        temp /= 10;
    } while (temp > 0);

    for (int i = digits - 1; i >= 0; i--) {
        buffer[index + i] = (number % 10) + '0';
        number /= 10;
    }

    index += digits;
    buffer[index++] = '\n';
    buffer[index] = '\0';

    UART_SendString(buffer);
}


// Function to handle commands
void ProcessCommand(const char *command) {
    if (strcmp(command, "LAMP") == 0) {
        Lamp_Toggle('A', 2);
        UART_SendString("LAMP TOGGLED\n");
    } else if (strcmp(command, "PLUG") == 0) {
        Plug_Toggle('D', 0);
        UART_SendString("PLUG TOGGLED\n");
    } else if (strcmp(command, "STATUS") == 0) {
        UART_SendString("SYSTEM STATUS: OK\n");
    } else if (strcmp(command, "DOOR") == 0) {
        if (Get_Door_Status('C', 4) == DOOR_OPEN) {
            UART_SendNumber(1);
        } else {
            UART_SendNumber(0);
        }
    } else if (strcmp(command, "COUNT") == 0) {
        UART_SendNumber(Get_Door_OpenCount('C', 4));
    } else if (strcmp(command, "TEMP") == 0) {
        UART_SendNumber(Get_Temperature());
    } else {
        UART_SendString("UNKNOWN COMMAND\n");
    }
}

int main(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    Lamp_Init('A', 2);
    Door_Init('C', 4);
    LM35_Init();
    Plug_Init('D', 0);
    Buzzer_init();

    UART_Init();

    char commandBuffer[32];
    uint32_t commandIndex = 0;

    while (1) {
        while (UARTCharsAvail(UART_BASE)) {
            char c = UARTCharGet(UART_BASE);

            if (c == '\n' || c == '\r') {
                commandBuffer[commandIndex] = '\0';
                if (commandIndex > 0) {
                    ProcessCommand(commandBuffer);
                }
                commandIndex = 0;
            } else if (commandIndex < sizeof(commandBuffer) - 1) {
                commandBuffer[commandIndex++] = c;
            }
        }
        Update_Door_OpenCount('C', 4);
        if (Get_Temperature() > 60) {
            Buzzer_activate();
        } else {
            Buzzer_deactivate();
        }

        SysCtlDelay(SysCtlClockGet() / 3);  // 1-second delay
    }
}
