#include "Elevator.h"
#include "BOARD.h"

typedef enum {
    UNINIT_STATE,
    HOME_STATE,
    ATM6_STATE,
    REN_STATE,
    SAFE_STATE,
} ElevatorState_t;

uint8_t InitElevator(void) {
    return SUCCESS
}

uint8_t ElevatorHome(void) {
    return SUCCESS;
}

uint8_t LiftToAtM6(void) {
    return SUCCESS;
}

uint8_t LiftToRen(void) {
    return SUCCESS;
}

uint8_t LiftToSafe(void) {
    return SUCCESS;
}