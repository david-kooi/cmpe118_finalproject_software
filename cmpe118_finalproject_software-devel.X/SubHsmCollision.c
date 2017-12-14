/*
 * File: CollisionSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Collision file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunCollisionSM()
 * 10/23/11 18:20 jec      began conversion from SMCollision.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "HsmTopLevel.h"
#include "SubHsmCollision.h"
#include "TapeSensorEventChecker.h"
#include "Trajectory.h"
#include "DriveService.h"
#include "BeaconEventChecker.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    IDLE_STATE,
    INIT_STATE,
    COLLISION_AVOID,
    REN_APPROACH,
} CollisionSubHSMState_t;

static const char *StateNames[] = {
	"IDLE_STATE",
	"INIT_STATE",
	"COLLISION_AVOID",
	"REN_APPROACH",
};


static uint8_t maneuverStep;
static uint8_t pastRenShip;
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static CollisionSubHSMState_t CurrentState = IDLE_STATE;
static uint8_t MyPriority;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitCollisionSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunCollisionFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitCollisionSubHSM(void) {
    ES_Event returnEvent;

    pastRenShip = 0;
    //printf("COLLISION INIT\r\n");
    CurrentState = INIT_STATE;
    returnEvent = RunCollisionSubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunCollisionSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunCollisionSubHSM(ES_Event ThisEvent) {
    static uint8_t snubTimerExpired = 0;
    
    uint8_t makeTransition = FALSE; // use to flag transition
    CollisionSubHSMState_t nextState; // <- change type to correct enum
    ES_Tattle(); // trace call stack
    
    
    switch (CurrentState) {
        case IDLE_STATE:
            break;

        case INIT_STATE:
        {// If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT) {

                printf("START TRAJ\r\n");
                
            
            
                if(atm6KillCount == 3){

                }else{
                    printf("COLLISION SWITCH\r\n");
                    SWITCH_STATE(COLLISION_AVOID);
                    ES_Timer_InitTimer(COLLISION_TAPE_SNUB_TIMER, 3000);
                }
            }
            
        }   
            
            break;
        case COLLISION_AVOID:
        {
            ON_ENTRY{
                InitBackwardTrajectory(step2Inches);   
                maneuverStep = 1;
            }
            
            
            if(ThisEvent.EventType == TRAJECTORY_COMPLETE){
                switch(maneuverStep){
                    case 1:
                        InitBackwardTrajectory(pivot90Degrees);
                        maneuverStep++;
                        break;
                    case 2:
                        SetForwardSpeed(10000);
                        SetTurnRadius(10000);
                        maneuverStep++;
                        break;
                    default:
                        break;
                }
            }
             
            if(ThisEvent.EventType == FL_BUMPER_ON){
                StopDrive();
                SWITCH_STATE(COLLISION_AVOID);
            }
            
            if(ThisEvent.EventType == ES_TIMEOUT){
                if(ThisEvent.EventParam == COLLISION_TAPE_SNUB_TIMER){
                    snubTimerExpired = 1;
                }
            }
            if(ThisEvent.EventType == TS_CENTER_ON_TAPE && snubTimerExpired){
                StopDrive();
                SetForwardSpeed(5000);
                SetTurnRadius(-6000);
                
                ES_Event ReturnEvent;
                ReturnEvent.EventType = COLLISION_COMPLETE;
                PostHsmTopLevel(ReturnEvent);
                
                CurrentState = IDLE_STATE;
                InitTapeFollowSubHSM();  
                
                snubTimerExpired = 0;
            }
            
            
            
            break;
        }   
            
        case REN_APPROACH:
        {
            ON_ENTRY{
                maneuverStep = 1;
                InitBackwardTrajectory(step2Inches);   
            }
            
            
            
            
            
            if(ThisEvent.EventType == TRAJECTORY_COMPLETE){
                switch(maneuverStep){
                    case 1:
                        // Raise evavator to check for beacon
                        
                        maneuverStep++;
                        break;
                    case 2:
                        InitBackwardTrajectory(pivot45Degrees);
                        maneuverStep++;
                        break;
                    case 3:
                        InitBackwardTrajectory(pivot5Degrees);
                        maneuverStep++;
                        break;
                    case 4:
                        InitForwardTrajectory(step8Inches);
                        maneuverStep++;
                        break;
                    case 5:
                        InitForwardTrajectory(pivot45Degrees);
                        maneuverStep++;
                        break;
                    case 6:
                        InitForwardTrajectory(step10Inches);
                        maneuverStep++;
                        break;
                    case 7:
                        InitForwardTrajectory(step5Inches);
                    case 8:
                        InitForwardTrajectory(pivot90Degrees);
                        maneuverStep++;
                        break;
                    case 9:
                        SetForwardSpeed(10000);
                        maneuverStep++;
                    default:
                        break;
                }
            }
            
            // Check beacon levels
            if(ThisEvent.EventType == ELEVATOR_ARRIVED){
                uint16_t beaconVal = ReadBeacon();
                if(beaconVal > HI_HEAD_ON_THRESH){
                    // Send a false TRAJECTORY_COMPLETE event to continue maneuver
                    ES_Event ReturnEvent;
                    ReturnEvent.EventType = TRAJECTORY_COMPLETE;
                    PostHsmTopLevel(ReturnEvent);
                }
            }
            
            if(ThisEvent.EventType == FR_BUMPER_ON || ThisEvent.EventType == FL_BUMPER_ON){
                StopDrive();
                SWITCH_STATE(COLLISION_AVOID);
            }
            
            // Ren ship approach
            if(ThisEvent.EventType == TS_REAR_ON_TAPE){
                StopDrive();
               
            }
            
            
            
            ON_EXIT{
                
            }
            break;
        }
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunCollisionSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunCollisionSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}

void COL_SetIdle(void) {
    CurrentState = IDLE_STATE;
}

/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

