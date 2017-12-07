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
#include "serial.h"
#include "DriveService.h"
#include "Trajectory.h"
//#include sub-sm's here
#include "SubHsmTapeFollow.h"
#include "SubHsmTrackWireAlign.h"
#include <stdio.h>
#include "Elevator.h"


extern Trajectory pivot90Degrees;
extern Trajectory pivot180Degrees;
extern Trajectory step5Inches;

typedef enum {
    INIT,
    STARTUP,
    DESTROYING_ATM6,
    DESTROYING_REN,
    IDLE,
} TopLevelState;

typedef enum{
    AT_TAPE_FOLLOW,
    AT_ALIGN_MANUVER,
} ATM6State;

static const char *StateNames[] = {
    "INIT",
    "STARTUP",
    "TOP_ATM6",
    "DESTROYING_ATM6",
    "DESTROYING_REN",
    "IDLE",

};

static TopLevelState CurrentState;
static uint8_t myPriority;

uint8_t InitHsmTopLevel(uint8_t priority) {
    EnableDriveMotors();
    
    myPriority = priority;
    CurrentState = INIT;
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
    
    
    static ATM6State ATM6_STATE = AT_TAPE_FOLLOW;
    
    ThisEvent = RunTrackWireAlignSubHSM(ThisEvent);
    ThisEvent = RunTapeFollowSubHSM(ThisEvent);
    
    switch (CurrentState) {
        case INIT:
            if (ThisEvent.EventType == ES_INIT) {
                printf("Initializing Top Level State Machine\r\n");
                
                //                SetForwardSpeed(MAX_FORWARD_SPEED);
                //                SetTurningSpeed(0);
                InitElevator();
                LiftToAtM6();

                SWITCH_STATE(STARTUP);

            }

            ON_EXIT{

            }
            break;
        case STARTUP:
            ON_ENTRY
        {
//            InitTrackWireAlignSubHSM();
            
            //SWITCH_STATE(DESTROYING_ATM6);
        }

            ON_EXIT{
                SetForwardSpeed(MAX_FORWARD_SPEED);
            }
            break;
        case DESTROYING_ATM6:
            
            ON_ENTRY{
                InitTapeFollowSubHSM();
                ATM6_STATE = AT_TAPE_FOLLOW;
            }
            switch(ATM6_STATE){
                case AT_TAPE_FOLLOW:
                    
                    if(ThisEvent.EventType == TW_RIGHT_TOUCHING){
                        printf("RECEIVED EVEN\r\n");
                        InitTrackWireAlignSubHSM();
                        ATM6_STATE = AT_ALIGN_MANUVER;
                    }
                    

                    break;
                case AT_ALIGN_MANUVER:
                    
                    
                    
                    
                    break;
                default:
                    break;
            }
            


            break;
        case DESTROYING_REN:
            break;
        case IDLE:
            DisableDriveMotors();
            break;

        default:
            break;
    }
    if (makeTransition) {
        printf("Make Transistion\r\n");
        IsTransmitEmpty();
        RunHsmTopLevel(EXIT_EVENT);
        printf("After EXIT_EVENT\r\n");
        IsTransmitEmpty();
        CurrentState = nextState;
        RunHsmTopLevel(ENTRY_EVENT);
        printf("After ENTRY_EVENT\r\n");
        IsTransmitEmpty();
    }
    ES_Tail();
    return ThisEvent;
}
