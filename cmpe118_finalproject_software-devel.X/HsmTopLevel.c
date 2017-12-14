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
#include "DeployerDriver.h"
#include "DriveService.h"
#include "Trajectory.h"
//#include sub-sm's here
#include "SubHsmTapeFollow.h"
#include "SubHsmTrackWireAlign.h"
#include "SubHsmCollision.h"
#include <stdio.h>
#include "ElevatorService.h"


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
    AT_COLLISION_AVOID,
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
    
    static uint8_t maneuverStep = 0;
    static ATM6State    currATState      = AT_TAPE_FOLLOW;
    static StartupState currStartupState =  ST_FIND_BEACON;
    
    ThisEvent = RunTrackWireAlignSubHSM(ThisEvent);
    ThisEvent = RunTapeFollowSubHSM(ThisEvent);
    ThisEvent = RunCollisionSubHSM(ThisEvent);
    
    switch (CurrentState) {
        case INIT:
            if (ThisEvent.EventType == ES_INIT) {
                printf("Initializing Top Level State Machine\r\n");
                
//                                SetForwardSpeed(MAX_FORWARD_SPEED);
                //                SetTurningSpeed(0);
                
                //LiftToAtM6();
                atm6KillCount = 0;
                EnableDriveMotors();
                SWITCH_STATE(STARTUP);

            }

            ON_EXIT{

            }
            break;
        case STARTUP:
            ON_ENTRY
            {
                printf("STARTUP INIT\r\n");
                
                SetTurningSpeed(130);
//                ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1500);
                currStartupState = ST_FIND_BEACON;
            }
            switch(currStartupState){
                case ST_FIND_BEACON:
                {
                    if(ThisEvent.EventType == BC_HEAD_ON || ThisEvent.EventType == BC_IN_SIGHT){
                        StopDrive();
                        SetForwardSpeed(-5000); // Backup
                        currStartupState = ST_FIND_TAPE;
                    }
                    break;
                }
                case ST_FIND_TAPE:
                {
                    if(ThisEvent.EventType == TS_REAR_ON_TAPE){
                        
                        StopDrive();
                        SetTurningSpeed(200);
                        
                    }
                    
                    if(ThisEvent.EventType == TS_CENTER_ON_TAPE){
                        StopDrive();
                        SWITCH_STATE(DESTROYING_ATM6);
                    }
                    
                    break;
                }
            }
//            if(ThisEvent.EventType == ES_TIMEOUT){
//                SWITCH_STATE(DESTROYING_ATM6);
//            }
            
            
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
                        TS_SetIdle();
                        StopDrive();
                        InitTrackWireAlignSubHSM();
                        currATState = AT_ALIGN_MANUVER;
//                        TwSampleOff();
                    }else if(ThisEvent.EventType == FR_BUMPER_ON || ThisEvent.EventType == FL_BUMPER_ON){
                        TS_SetIdle();
                        StopDrive();
                        InitCollisionSubHSM();
                        currATState = AT_COLLISION_AVOID;
                    }
                    
                    break;
                    
                case AT_COLLISION_AVOID:
                    if(ThisEvent.EventType == COLLISION_COMPLETE){
                        currATState = AT_TAPE_FOLLOW;
                    }
                    break;
                case AT_ALIGN_MANUVER:
                    
                    switch(ThisEvent.EventType){
                        case ES_TIMEOUT:
                            if(ThisEvent.EventParam == TW_FINAL_TO){
                                TW_SetIdle();
                                InitTapeFollowSubHSM();
                                currATState = AT_TAPE_FOLLOW;
                            }
                            break;
                        case AT_KILLED: // Finished with atm6 destroy
                            TW_SetIdle();
                            currATState = AT_RETURN_TO_TAPE;
                            InitBackwardTrajectory(step2Inches);
                            atm6KillCount++;
                            break;
                        default:
                            break;
                    }
                
                    

                    break;
                    
                case AT_RETURN_TO_TAPE:
                    
                    
                    if(ThisEvent.EventType == TRAJECTORY_COMPLETE){

                        SetTurningSpeed(-100); 
                        ElevatorHome();
                        
                    }
                    
                    if(ThisEvent.EventType == TS_RIGHT_ON_TAPE){
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
