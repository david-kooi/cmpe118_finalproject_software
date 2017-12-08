#include "DeployerDriver.h"
#include "ES_Events.h"
#include "ES_Configure.h"
#include "ES_Timers.h"


typedef enum{
    LOAD,
    DISPENSE,
    RESET,
} DeployerState;

static uint8_t isDispensing = 0;


void DispenseBall(void){
    
    // Do not continue if we are in action
    if(isDispensing){
        return;
    }
    
    isDispensing = 1;
    ES_Timer_InitTimer(BALL_DRIVER_TIMER, ACTION_PERIOD);
}


uint8_t InitDeployerDriver(uint8_t priority) {
    myPriority = priority;
    
    ES_Event thisEvent;
    thisEvent.EventType = ES_INIT;
    return ES_PostToService(myPriority, thisEvent);
}

uint8_t PostDeployerDriver(ES_Event thisEvent) {
    return ES_PostToService(myPriority, thisEvent);
}

uint8_t RunBallDispenser(ES_Event timerEvent){
    static DeployerState currState = LOAD;
    
    if(timerEvent.EventType == ES_TIMEOUT){
        if(timerEvent.EventParam == BALL_DRIVER_TIMER){
            switch(currState){
                case LOAD:
                   RC_SetPulseTime(RC_PORTX04, LOAD_PWM);
                    
                    currState = DISPENSE;
                    ES_Timer_InitTimer(BALL_DRIVER_TIMER, ACTION_PERIOD);
                    
                     break;
                case DISPENSE:
                    
                    RC_SetPulseTime(RC_PORTX04, DISPENSE_PWM);
                    currState = RESET;
                    ES_Timer_InitTimer(BALL_DRIVER_TIMER, ACTION_PERIOD);
                    
                    break;
                case RESET:
                    
                    RC_SetPulseTime(RC_PORTX04, LOAD_PWM);
                    isDispensing = 0;
                    break;  
            }
        }
    }
}


