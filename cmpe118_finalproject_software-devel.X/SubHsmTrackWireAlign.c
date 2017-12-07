/*
 * File: TrackWireAlignSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * TrackWireAlign file to set up a Heirarchical State Machine to work with the Events and
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
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTrackWireAlignSM()
 * 10/23/11 18:20 jec      began conversion from SMTrackWireAlign.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "HsmTopLevel.h"
#include "SubHsmTrackWireAlign.h"
#include "TrackWireEventChecker.h"
#include <stdio.h>
#include "DriveService.h"
#include "Trajectory.h"
extern Trajectory pivot180degrees;
extern Trajectory pivot90Degrees;
extern Trajectory step10Inches;
//extern Trajectory step5inches;

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define INIT_TURNING_SPEED 45
#define CENTER_TW_VAL 600

typedef enum {
    IDLE_STATE,
    INIT_STATE,
    ORIENT_STATE,
    ALIGN_STATE,
} TrackWireAlignSubHSMState_t;

static const char *StateNames[] = {
	"IDLE_STATE",
	"INIT_STATE",
	"ORIENT_STATE",
	"ALIGN_STATE",
};


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

static TrackWireAlignSubHSMState_t CurrentState = IDLE_STATE; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTrackWireAlignSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTrackWireAlignFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitTrackWireAlignSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = INIT_STATE;
    returnEvent = RunTrackWireAlignSubHSM(INIT_EVENT);



    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunTrackWireAlignSubHSM(ES_Event ThisEvent)
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
ES_Event RunTrackWireAlignSubHSM(ES_Event ThisEvent) {
    static ES_Event dtEvent; // Derivative event
    
    static uint16_t rightValue = 0;
    static uint16_t leftValue = 0;
    static int maneuverStep;
    static int parity = 0;

    uint8_t makeTransition = FALSE; // use to flag transition
    TrackWireAlignSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case IDLE_STATE:
            break;
        case INIT_STATE: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                SWITCH_STATE(ORIENT_STATE);
                //SWITCH_STATE(ALIGN_STATE);
            }
            break;
            
        case ORIENT_STATE: // in the first state, replace this with correct names
            ON_ENTRY
            {
                SetTurningSpeed(20);
                dtEvent.EventType = TW_START_DERIVATIVE;
                dtEvent.EventParam = 0;
                PostRateGroupDriverService(dtEvent);
                parity = 1; // Toggle parity
            }
            
            switch(ThisEvent.EventType){
                 case TW_NULL_DERIVATIVE:
                    dtEvent.EventType = TW_START_DERIVATIVE;
                    dtEvent.EventParam = 0;
                    PostRateGroupDriverService(dtEvent);
                    break;
                case TW_ZERO_DERIVATIVE:
                    StopDrive();
                    break;
            }
                    
            
            break; //ORIENT_STATE
            
            
        case ALIGN_STATE:
            ON_ENTRY
        {
            maneuverStep = 1;
            InitBackwardTrajectory(pivot45Degrees);
        }
            if (ThisEvent.EventType == TRAJECTORY_COMPLETE) {
                switch (maneuverStep) {
                    case 1:
                        InitForwardTrajectory(step5Inches);
                        break;
                    case 2:
                        InitForwardTrajectory(pivot135Degrees);
                        break;
                    case 3:
                        InitForwardTrajectory(step5Inches);
                        break;
                    case 4:
                        StopDrive();
                        SWITCH_STATE(ORIENT_STATE);
                        break;
                    default:
                        break;

                }
                maneuverStep++;
            }
            //GAME PLAN
            //forward trajectory 10 inches linear
            //right 90 degrees
            //forward 5 inches
            //180 degrees
            //nextstate = orientstate
            
            
            ON_EXIT{
                SetForwardSpeed((MAX_FORWARD_SPEED) / 2);
            }
            break;

        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunTrackWireAlignSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunTrackWireAlignSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

