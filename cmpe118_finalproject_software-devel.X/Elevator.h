#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "ES_Configure.h"
#include "IO_Ports.h"
#include "Stepper.h"
#include <stdint.h>

#define ELEVATOR_STEP_RATE 200

#define HOME_HEIGHT 10
#define ATM6_HEIGHT 400
#define REN_HEIGHT 775
#define SAFE_HEIGHT 600

#define POST_ELEVATOR_EVENT PostHsmTopLevel

/* Periodic Drive Service Functions */
uint8_t InitElevator(void);

/* Public Interface Functions */
uint8_t ElevatorHome(void);
uint8_t LiftToAtM6(void);
uint8_t LiftToRen(void);
uint8_t LiftToSafe(void);
#endif  /* DRIVESUBSM_H */