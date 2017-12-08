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
extern Trajectory pivot135Degrees;
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
    AT_RETURN_TO_TAPE,
} ATM6State;

typedef enum{
    ST_FIND_BEACON,
    ST_FIND_TAPE,
    
} StartupState;

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
    
    
    static ATM6State    currATState = AT_TAPE_FOLLOW;
    static StartupState currStartupState =  ST_FIND_BEACON;
    
    
    ThisEvent = RunTrackWireAlignSubHSM(ThisEvent);
    ThisEvent = RunTapeFollowSubHSM(ThisEvent);
      
    
      
    switch (CurrentState) {
        case INIT:
            if (ThisEvent.EventType == ES_INIT) {
                printf("Initializing Top Level State Machine\r\n");
                EnableDriveMotors();
                
//                                SetForwardSpeed(MAX_FORWARD_SPEED);
                //                SetTurningSpeed(0);
                
                //LiftToAtM6();

                
                SWITCH_STATE(STARTUP);

            }

            ON_EXIT{

            }
            break;
        case STARTUP:
            ON_ENTRY
            {
                SetTurningSpeed(120);
            
            }
            
            switch(currStartupState){
                case ST_FIND_BEACON:
                    if(ThisEvent.EventType == BC_HEAD_ON || ThisEvent.EventType == BC_IN_SIGHT){
                        StopDrive();
                        SetForwardSpeed(-5000); // Backup
                        currStartupState = ST_FIND_TAPE;
                    }
                    break;
            
                case ST_FIND_TAPE:
                    if(ThisEvent.EventType == TS_REAR_ON_TAPE){
                        
                        StopDrive();
                        SetTurningSpeed(120);
                        
                    }
                    
                    if(ThisEvent.EventType == TS_CENTER_ON_TAPE){
                        StopDrive();
                        SWITCH_STATE(DESTROYING_ATM6);
                    }
                    
                    break;
                
            }
            
            
            
            ON_EXIT{
                
            }
            break;
        case DESTROYING_ATM6:
            
            ON_ENTRY{       
                InitTapeFollowSubHSM();
                currATState = AT_TAPE_FOLLOW;
            }
            switch(currATState){
                case AT_TAPE_FOLLOW:
                    
                    if(ThisEvent.EventType == TW_RIGHT_IN_SIGHT){
                        StopDrive();
                        TS_SetIdle();
                        InitTrackWireAlignSubHSM();
                        currATState = AT_ALIGN_MANUVER;
                    }
                    

                    break;
                case AT_ALIGN_MANUVER:
                    
                    switch(ThisEvent.EventType){
                        case TW_LEFT_OFF: // Finished with at destroy
                            InitBackwardTrajectory(step2Inches);
                            currATState = AT_RETURN_TO_TAPE;
                            break;
                    }
                    

                    break;
                    
                case AT_RETURN_TO_TAPE:
                    if(ThisEvent.EventType == TRAJECTORY_COMPLETE){
                        InitBackwardTrajectory(pivot90Degrees);
                        
                        InitTapeFollowSubHSM();
                        currATState = AT_TAPE_FOLLOW;
                    }
                   
                    
                    
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
