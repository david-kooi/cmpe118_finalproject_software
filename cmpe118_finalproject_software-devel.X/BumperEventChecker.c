#include "BumperEventChecker.h"

#include <stdio.h>



// CheckBumper defines
#define NUM_BUMPERS 4
#define DEBOUNCE_LENGTH 5
#define ON_EVENT  0
#define OFF_EVENT 1

/* Module variables for the CheckBumper function. */

/* Standardizes the index for bumper access. 
   These numbers are mapped to the 4-bit value returned by ReadBumpers()*/
enum BumperIndex{RR_BUMPER = 8, RL_BUMPER=4, FR_BUMPER = 2, FL_BUMPER = 1};
#define RR_BUMPER_MASK 0x08
#define RL_BUMPER_MASK 0x04
#define FR_BUMPER_MASK 0x02
#define FL_BUMPER_MASK 0x01


#define RR_BUMPER_PIN PIN10
#define RL_BUMPER_PIN PIN8
#define FR_BUMPER_PIN PIN6
#define FL_BUMPER_PIN PIN4

#define RR_BUMPER_NUM PORTZ, RR_BUMPER_PIN
#define RL_BUMPER_NUM PORTZ, RL_BUMPER_PIN
#define FR_BUMPER_NUM PORTZ, FR_BUMPER_PIN
#define FL_BUMPER_NUM PORTZ, FL_BUMPER_PIN


// Map the bumper event constants to BumperIndex
// Initialized in InitializeBumperCheck()
ES_EventTyp_t bumperEvents[RR_BUMPER][2]; // Array must index up to RR_BUMPER 
                                           

/* A struct containing bumper information. */
typedef struct{
    uint8_t bumperMask;
    uint8_t lastBumperState;
    uint8_t numDebounce;
    ES_EventTyp_t onEvent;
    ES_EventTyp_t offEvent;
}BumperState_t;

/* An array for bumper access. Initialized in InitializeBumperCheck() */
BumperState_t* bumperStateArray[RR_BUMPER];// Array must index up tp RR_BUMPER
/*
 * currentBumperStates shall use the four LSB for indicating on/off status.
 * All bumpers on:  0000.1111 
 * All bumpers off: 0000.0000
 * Bumper masks are defined above in the BumperIndex enum
 * 
*/
static uint8_t currentBumperStates = 0x00; // Initialize in InitializeBumperEventChecker()

/**
 * @Function ReadBumpers(void)
 * @param none
 * @return uint8_t with bumper states encoded as bits

 * @brief This function checks all bumpers in sequence. Events are generated 
 *        for each bumper pressed. TRUE is returned if any bumper event was
 *        generated.
 * @note 
 * @author David Kooi, Ben Swandon, 2017.10.3 14:08
 * @modified */
uint8_t ReadBumpers(){
    uint8_t bumperState = 0;
    
    uint16_t portState = IO_PortsReadPort(PORTY);
    
    //printf("PORT STATE: 0x%x\r\n",portState);
    
    // If the BUMPER_PIN is greater than 0, toggle the bumperState bit. Else, do nothing
    (~portState & RR_BUMPER_PIN) ? (bumperState ^= RR_BUMPER_MASK) : (bumperState = bumperState);
    (~portState & RL_BUMPER_PIN) ? (bumperState ^= RL_BUMPER_MASK) : (bumperState = bumperState);
    (~portState & FR_BUMPER_PIN) ? (bumperState ^= FR_BUMPER_MASK) : (bumperState = bumperState);
    (~portState & FL_BUMPER_PIN) ? (bumperState ^= FL_BUMPER_MASK) : (bumperState = bumperState);

    return bumperState;
}


/**
 * @Function CheckBumpers(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function checks all bumpers in sequence. Events are generated 
 *        for each bumper pressed. TRUE is returned if any bumper event was
 *        generated.
 * @note 
 * @author David Kooi, Ben Swandon, 2017.10.3 14:08
 * @modified */
uint8_t CheckSpecificBumper(BumperState_t* bumperState); // Forward Declare
uint8_t CheckBumpers(void) {  
    
    // TODO: Create ReadBumpersFunction
    
    uint8_t returnVal = FALSE;
    currentBumperStates = ReadBumpers(); // Get current bumper state
    
    // Check each specific bumper
    returnVal |= CheckSpecificBumper(bumperStateArray[FR_BUMPER]);
    returnVal |= CheckSpecificBumper(bumperStateArray[FL_BUMPER]);
    returnVal |= CheckSpecificBumper(bumperStateArray[RR_BUMPER]);
    returnVal |= CheckSpecificBumper(bumperStateArray[RL_BUMPER]);
      
    
    // Make sure the TRUE/FALSE macros are used for the return value
    if(returnVal){
        return TRUE;
    }else{
        return FALSE;
    }
}
/**
 * @Function CheckSpecificBumper(BumperState_t*)
 * @param BumperState_t*
 * @return TRUE if an event occured or FALSE if an event did not occur
 * @brief Checks the bumper defined in by the BumperState_t argument
 * @author David Kooi, 2017.10.03 15:11
 * @modified 
 */
uint8_t CheckSpecificBumper(BumperState_t* bumperState){
    ES_Event thisEvent;
    BUMPER_PRINT("CHECKING BUMPER: %d\n", bumperState->bumperMask);
    
    // Get the current state of a specific bumper
    uint8_t specificBumperState = currentBumperStates & bumperState->bumperMask;
    BUMPER_PRINT("STATE: %d\n", specificBumperState);
    BUMPER_PRINT("DEBOUNCE: %d\n", bumperState->numDebounce);
    
    // Check if the current state is different than the last state
    if(specificBumperState != bumperState->lastBumperState){
        BUMPER_PRINT("CHANGE IN SWITCH\n");
        // If yes, wait until change is positive for DEBOUNCE_LENGTH times
        bumperState->numDebounce = bumperState->numDebounce + 1;
        
        // Debounce passed, post the event
        if(bumperState->numDebounce == DEBOUNCE_LENGTH){
            BUMPER_PRINT("EVENT!!!!!\n");
            
            // Reset debounce
            bumperState->numDebounce = 0;  
            // Set last state to current state
            bumperState->lastBumperState = specificBumperState;
            
            // Check if bumper has turned on or off
            // And configure the event
            if(bumperState->lastBumperState == 0){
                // We have gone from on to off
                thisEvent.EventType = bumperState->offEvent;
            }else{
                // We have gone from off to on
                thisEvent.EventType = bumperState->onEvent;
            }  
            thisEvent.EventParam = bumperState->lastBumperState;
            
            BUMPER_PRINT("EVENT: %s\n", EventNames[thisEvent.EventType]);
            
            #ifdef TEST_HARNESS_SERVICE_TEST           // keep this as is for test harness
                PostTestHarnessService(thisEvent);
            #else
                PostHsmTopLevel(thisEvent);
            #endif   
                return TRUE;
            
        }
        
    }else{ // Current state is the same as last state
        // Reset debounce
        bumperState->numDebounce = 0;
        return FALSE;
    }
    
    return FALSE;
    
}

/**
 * @Function InitializeBumperChecker(void)
 * @param none
 * @return none
 * @brief Initializes variables related to the CheckBumper function.
 * @author David Kooi, 2017.10.03 14:30
 * @modified 
 */
void InitializeBumperChecker(void){
    BUMPER_PRINT("INIT BUMPERCHECKER\n");
            
    // Initialize Hardware
    IO_PortsSetPortInputs(RR_BUMPER_NUM);
    IO_PortsSetPortInputs(RL_BUMPER_NUM);
    IO_PortsSetPortInputs(FR_BUMPER_NUM);
    IO_PortsSetPortInputs(FL_BUMPER_NUM);
    
    
    // Initialize bumperEvents
    bumperEvents[FR_BUMPER][ON_EVENT]  = FR_BUMPER_ON;
    bumperEvents[FR_BUMPER][OFF_EVENT] = FR_BUMPER_OFF;

    bumperEvents[FL_BUMPER][ON_EVENT]  = FL_BUMPER_ON;
    bumperEvents[FL_BUMPER][OFF_EVENT] = FL_BUMPER_OFF;

    bumperEvents[RR_BUMPER][ON_EVENT]  = RR_BUMPER_ON;
    bumperEvents[RR_BUMPER][OFF_EVENT] = RR_BUMPER_OFF;

    bumperEvents[RL_BUMPER][ON_EVENT]  = RL_BUMPER_ON;
    bumperEvents[RL_BUMPER][OFF_EVENT] = RL_BUMPER_OFF;
    BUMPER_PRINT("BUMPER EVENTS SET\n");
    // Initialize the 4 bit bumpers state
    //currentBumperStates = ReadBumpers(); // TODO
    
    // Allocate the BumperState_t structs
    int mask = 1;
    int i, idx;
    
    // BumperState_t indecies are mapped to the 
    // 1-hot encoding of the bumpers
    //{RR_BUMPER = 8, RL_BUMPER=4, FR_BUMPER = 2, FL_BUMPER = 1};
    // 0001: FL
    // 0010: FR
    // 0100: RL
    // 1000: RR
    for(i = 0; i < NUM_BUMPERS; i++){
 
        idx = mask << i;
        BUMPER_PRINT("Initializing BumperState_t %d\n", i);
        bumperStateArray[idx] = (BumperState_t*)malloc(sizeof(BumperState_t));
        
        // Shift mask every iteration
        bumperStateArray[idx]->bumperMask = mask << i; 
        // Mask the current bumper value
        bumperStateArray[idx]->lastBumperState = currentBumperStates & bumperStateArray[idx]->bumperMask;
        // No debounce yet, initialize as 0
        bumperStateArray[idx]->numDebounce = 0;
        // Setup the on and off events
        bumperStateArray[idx]->onEvent  = bumperEvents[idx][ON_EVENT];
        bumperStateArray[idx]->offEvent = bumperEvents[idx][OFF_EVENT]; 
        BUMPER_PRINT("ON EVENT: %s\n", EventNames[bumperEvents[idx][ON_EVENT]]);
        BUMPER_PRINT("OFF EVENT: %s\n", EventNames[bumperEvents[idx][OFF_EVENT]]);

        
        BUMPER_PRINT("bumperMask %d\n", bumperStateArray[idx]->bumperMask);
        
        
    }
    BUMPER_PRINT("CHECKBUMPER INIT DONE\n");
    
}