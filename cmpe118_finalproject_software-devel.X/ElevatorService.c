#include "ES_Configure.h"
#include "ES_Framework.h"
//#include "ES_Events.h"
#include "Elevator.h"
#include "BOARD.h"
#include "Stepper.h"
#include "ES_Timers.h"
#include <stdio.h>

#define DIRECTION(x) (((x) < 0) ? (REVERSE) : (FORWARD))
#define UP FORWARD
#define DOWN REVERSE
#define ABS(x) (((x) < 0) ? (-(x)) : (x))

#define RELEASE_RAMP_STEPS 20
#define FIND_ZERO_STEPS -20

typedef enum {
    INIT_STATE,
    HOME_STATE,
    ATM6_STATE,
    REN_STATE,
    SAFE_STATE,
    BETWEEN_STATE,
} ElevatorState_t;

static const char *StateNames[] = {
	"INIT_STATE",
	"HOME_STATE",
	"ATM6_STATE",
	"REN_STATE",
	"SAFE_STATE",
	"BETWEEN_STATE",
};


//static const char *StateNames[] = {
//    "INIT_STATE",
//    "HOME_STATE",
//    "ATM6_STATE",
//    "REN_STATE",
//    "SAFE_STATE",
//    "BETWEEN_STATE",
//};

static uint8_t myPriority;

static ElevatorState_t currentState = INIT_STATE;
static ElevatorState_t nextState = INIT_STATE;
static uint8_t initStep;
static uint8_t wasSteppingLast;

static int16_t stepsToState(ElevatorState_t s);

uint8_t InitElevatorService(uint8_t priority) {
    myPriority = priority;
    Stepper_Init();
    Stepper_SetRate(ELEVATOR_STEP_RATE);
    currentState = INIT_STATE;
    initStep = 0;
    ES_Event thisEvent;
    Stepper_InitSteps(UP, RELEASE_RAMP_STEPS);
    wasSteppingLast = TRUE;
    ES_Timer_InitTimer(ELEVATOR_SERVICE_TIMER, 5);
    thisEvent.EventType = ES_INIT;
    return ES_PostToService(myPriority, thisEvent);
}

uint8_t PostElevatorService(ES_Event thisEvent) {
    return ES_PostToService(myPriority, thisEvent);
}

ES_Event RunElevatorService(ES_Event thisEvent) {
    ES_Event returnEvent;
    returnEvent.EventType = ES_NO_EVENT;
    uint8_t isSteppingNow;
    switch (currentState) {
        case INIT_STATE:
            switch (thisEvent.EventType) {
                case ES_TIMEOUT:
                    ES_Timer_InitTimer(ELEVATOR_SERVICE_TIMER, 5);
                    isSteppingNow = Stepper_IsStepping();
                    if (!isSteppingNow && wasSteppingLast) {
                        initStep++;
                        switch (initStep) {
                            case 1:
                                Stepper_InitSteps(DOWN, RELEASE_RAMP_STEPS + RUBBER_BAND_STEPS);
                                wasSteppingLast = TRUE;
                                break;
                            case 2:
                                Stepper_InitSteps(UP, HOME_HEIGHT);
                                wasSteppingLast = TRUE;
                                break;
                            case 3:
                                currentState = HOME_STATE;
                                wasSteppingLast = FALSE;
                                returnEvent.EventType = ELEVATOR_ARRIVED;
                                returnEvent.EventParam = currentState;
                                nextState = currentState;
                                POST_ELEVATOR_EVENT(returnEvent);
                                break;
                            default:
                                printf("You done goofed on elevator init!\r\n");
                                break;
                        }
                    }
                    wasSteppingLast = isSteppingNow;
                    break;
                default:
                    break;
            }
            break;
        case HOME_STATE:
        case ATM6_STATE:
        case REN_STATE:
        case SAFE_STATE:
            switch (thisEvent.EventType) {
                case ELEVATOR_STEP_COMMAND:
                    nextState = thisEvent.EventParam;
                    int16_t steps = stepsToState(nextState);
                    Stepper_InitSteps(DIRECTION(steps), ABS(steps));
                    wasSteppingLast = TRUE;
                    currentState = BETWEEN_STATE;
                    break;
                default:
                    break;
            }
            break;
        case BETWEEN_STATE:
            switch (thisEvent.EventType) {
                case ES_TIMEOUT:
                    ES_Timer_InitTimer(ELEVATOR_SERVICE_TIMER, 5);
                    isSteppingNow = Stepper_IsStepping();
                    if (!isSteppingNow && wasSteppingLast) {
                        currentState = nextState;
                        returnEvent.EventType = ELEVATOR_ARRIVED;
                        returnEvent.EventParam = currentState;
                        POST_ELEVATOR_EVENT(returnEvent);
                    }
                    wasSteppingLast = isSteppingNow;
                    break;
                default:
                    break;
            }
        default:
            break;
    }
    return returnEvent;
}

uint8_t ElevatorHome(void) {
    if (currentState == BETWEEN_STATE ||
            currentState == INIT_STATE ||
            currentState == HOME_STATE) {
        return ERROR;
    }
    ES_Event stepCommand;
    stepCommand.EventType = ELEVATOR_STEP_COMMAND;
    stepCommand.EventParam = HOME_STATE;
    return PostElevatorService(stepCommand);
}

uint8_t LiftToAtM6(void) {
    if (currentState == BETWEEN_STATE ||
            currentState == INIT_STATE ||
            currentState == ATM6_STATE) {
        return ERROR;
    }
    ES_Event stepCommand;
    stepCommand.EventType = ELEVATOR_STEP_COMMAND;
    stepCommand.EventParam = ATM6_STATE;
    return PostElevatorService(stepCommand);
}

uint8_t LiftToRen(void) {
    if (currentState == BETWEEN_STATE ||
            currentState == INIT_STATE ||
            currentState == REN_STATE) {
        return ERROR;
    }
    ES_Event stepCommand;
    stepCommand.EventType = ELEVATOR_STEP_COMMAND;
    stepCommand.EventParam = REN_STATE;
    return PostElevatorService(stepCommand);
}

uint8_t LiftToSafe(void) {
    if (currentState == BETWEEN_STATE ||
            currentState == INIT_STATE ||
            currentState == SAFE_STATE) {
        return ERROR;
    }
    ES_Event stepCommand;
    stepCommand.EventType = ELEVATOR_STEP_COMMAND;
    stepCommand.EventParam = SAFE_STATE;
    return PostElevatorService(stepCommand);
}

static int16_t stepsToState(ElevatorState_t s) {
    switch (currentState) {
        case INIT_STATE:
            return 0;
        case HOME_STATE:
            switch (s) {
                case INIT_STATE:
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
                case INIT_STATE:
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
                case INIT_STATE:
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
                case INIT_STATE:
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
