#include "Elevator.h"
#include "BOARD.h"
#include "Stepper.h"

#define DIRECTION(x) (((x) < 0) ? (REVERSE) : (FORWARD))
#define ABS(x) (((x) < 0) ? (-(x)) : (x))

typedef enum {
    UNINIT_STATE,
    HOME_STATE,
    ATM6_STATE,
    REN_STATE,
    SAFE_STATE,
} ElevatorState_t;

static const char *StateNames[] = {
	"UNINIT_STATE",
	"HOME_STATE",
	"ATM6_STATE",
	"REN_STATE",
	"SAFE_STATE",
};


static ElevatorState_t elevatorState = UNINIT_STATE;
static int16_t stepsToState(ElevatorState_t s);


uint8_t InitElevator(void) {
    if (elevatorState != UNINIT_STATE) {
        return ERROR;
    }
    Stepper_Init();
    Stepper_SetRate(ELEVATOR_STEP_RATE);
    Stepper_InitSteps(FORWARD, HOME_HEIGHT);
    elevatorState = HOME_STATE;
    return SUCCESS;
}

uint8_t ElevatorHome(void) {
    if (Stepper_IsStepping() || elevatorState == HOME_STATE) {
        return ERROR;
    }
    int32_t steps = stepsToState(HOME_STATE);
    Stepper_InitSteps(DIRECTION(steps), ABS(steps));
    elevatorState = HOME_STATE;
    return SUCCESS;
}

uint8_t LiftToAtM6(void) {
    if (Stepper_IsStepping() || elevatorState == ATM6_STATE) {
        return ERROR;
    }
    int32_t steps = stepsToState(ATM6_STATE);
    Stepper_InitSteps(DIRECTION(steps), ABS(steps));
    elevatorState = ATM6_STATE;
    return SUCCESS;
}

uint8_t LiftToRen(void) {
    if (Stepper_IsStepping() || elevatorState == REN_STATE) {
        return ERROR;
    }
    int32_t steps = stepsToState(REN_STATE);
    Stepper_InitSteps(DIRECTION(steps), ABS(steps));
    elevatorState = REN_STATE;
    return SUCCESS;
}

uint8_t LiftToSafe(void) {
    if (Stepper_IsStepping() || elevatorState == SAFE_STATE) {
        return ERROR;
    }
    int32_t steps = stepsToState(SAFE_STATE);
    Stepper_InitSteps(DIRECTION(steps), ABS(steps));
    elevatorState = SAFE_STATE;
    return SUCCESS;
}

static int16_t stepsToState(ElevatorState_t s) {
    switch (elevatorState) {
        case UNINIT_STATE:
            return 0;
        case HOME_STATE:
            switch (s) {
                case UNINIT_STATE:
                    return 0;
                case HOME_STATE:
                    return 0;
                break;
                case ATM6_STATE:
                    return ATM6_HEIGHT;
                break;
                case REN_STATE:
                    return REN_HEIGHT;
                break;
                case SAFE_STATE:
                    return SAFE_HEIGHT;
                break;
                default:
                    return 0;
            }
        break;
        case ATM6_STATE:
            switch (s) {
                case UNINIT_STATE:
                    return 0;
                case HOME_STATE:
                    return -ATM6_HEIGHT;
                break;
                case ATM6_STATE:
                    return 0;
                break;
                case REN_STATE:
                    return REN_HEIGHT - ATM6_HEIGHT;
                break;
                case SAFE_STATE:
                    return SAFE_HEIGHT - ATM6_HEIGHT;
                break;
                default:
                    return 0;
            }
        break;
        case REN_STATE:
            switch (s) {
                case UNINIT_STATE:
                    return 0;
                case HOME_STATE:
                    return -REN_HEIGHT;
                break;
                case ATM6_STATE:
                    return ATM6_HEIGHT - REN_HEIGHT;
                break;
                case REN_STATE:
                    return 0;
                break;
                case SAFE_STATE:
                    return SAFE_HEIGHT - REN_HEIGHT;
                break;
                default:
                    return 0;
            }
        break;
        case SAFE_STATE:
            switch (s) {
                case UNINIT_STATE:
                    return 0;
                case HOME_STATE:
                    return -SAFE_HEIGHT;
                break;
                case ATM6_STATE:
                    return ATM6_HEIGHT - SAFE_HEIGHT;
                break;
                case REN_STATE:
                    return REN_HEIGHT - SAFE_HEIGHT;
                break;
                case SAFE_STATE:
                    return 0;
                break;
                default:
                    return 0;
            }
        break;
        default:
        return 0;
    }
}
