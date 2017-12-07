#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "ES_Configure.h"
#include "IO_Ports.h"
#include "Stepper.h"
#include <stdint.h>

#define POST_ELEVATOR_EVENT PostHsmTopLevel

/* Periodic Drive Service Functions */
uint8_t InitElevator(void);

/* Public Interface Functions */
uint8_t ElevatorHome(void);
uint8_t LiftToAtM6(void);
uint8_t LiftToRen(void);
uint8_t LiftToSafe(void);
#endif  /* DRIVESUBSM_H */