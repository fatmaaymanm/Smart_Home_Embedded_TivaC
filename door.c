#include "door.h"

static uint32_t door_open_count = 0;
static uint8_t door_previous_state = 0;
static uint8_t door_current_state = 0;

void Door_Init(char port, uint8_t pin) {
    dio_init(port, pin, Input);
    door_open_count = 0;
    door_previous_state = dio_readpin(port, pin);
    door_current_state = door_previous_state;
}

uint8_t Get_Door_Status(char port, uint8_t pin) {
    door_current_state = dio_readpin(port, pin);
    return door_current_state;
}

void Update_Door_OpenCount(char port, uint8_t pin) {
  Get_Door_Status(port, pin);
  if (door_current_state == DOOR_OPEN && door_previous_state == DOOR_CLOSED) {
        door_open_count++;
    }
    door_previous_state = door_current_state;
}

uint32_t Get_Door_OpenCount(char port, uint8_t pin){
    return door_open_count;
}
