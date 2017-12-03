/* 
 * File:   HsmTopLevel.h
 * Author: jwloughb
 *
 * Created on December 2, 2017, 5:30 PM
 */

#include "BOARD.h"
#include "AD.h"
#include "roach.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "HsmTopLevel.h"
#include "DriveService.h"
//#include sub-sm's here
#include <stdio.h>

#define SWITCH_STATE(s) do{\
            nextState = s;\
            makeTransition = TRUE;\
            ThisEvent.EventType = ES_NO_EVENT;\
        } while(0)

typedef enum {
    IDLE,
    STARTUP,
    DESTROYING_ATM6,
    DESTROYING_REN,
    COMPLETE,
} TopLevelState;

static const char *StateNames[] = {
    "TOP_IDLE",
    "TOP_STRT",
    "TOP_ATM6",
    "TOP_REN",
    "TOP_DONE",
};

static TopLevelState CurrentState;

static uint8_t myPriority;

uint8_t InitHsmTopLevel(uint8_t priority) {
    myPriority = priority;
    CurrentState = IDLE;
    return ES_PostToService(myPriority, INIT_EVENT);
}

uint8_t PostHsmTopLevel(ES_Event thisEvent) {
    return ES_PostToService(myPriority, thisEvent);
}

ES_Event RunHsmTopLevel(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE;
    TopLevelState nextState;
    ES_Tattle();
    if (ThisEvent.EventType == BATTERY_DISCONNECTED && CurrentState != IDLE) {
        SWITCH_STATE(IDLE);
        printf("Battery disconnected, stop everything!\r\n");
    }
    switch (CurrentState) {
        case IDLE:
//            setMotors(5000,0);
//            setLeftMotor(5000);
            EnableDriveMotors();
            break;
        case STARTUP:
            break;
        case DESTROYING_ATM6:
            break;
        case DESTROYING_REN:
            break;
        default:
            break;
    }
    if (makeTransition) {
        RunHsmTopLevel(EXIT_EVENT);
        CurrentState = nextState;
        RunHsmTopLevel(ENTRY_EVENT);
    }
    ES_Tail();
    return ThisEvent;
}
