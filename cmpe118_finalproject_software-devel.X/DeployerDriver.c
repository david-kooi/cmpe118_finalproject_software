#include "DeployerDriver.h"
#include "ES_Events.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_Timers.h"
#include "serial.h"
#include <stdio.h>

typedef enum {
    LOAD,
    DISPENSE,
    RESET,
} DeployerState;


static uint8_t myPriority;
static uint8_t isDispensing = 0;
static DeployerState currState = RESET;

void DispenseBall(void) {

    // Do not continue if we are in action
    if (isDispensing) {
        return;
    }
    //    printf("DISPENSE\r\n");

    isDispensing = 1;
    currState = LOAD;
//    RC_SetPulseTime(RC_PORTX03, LOAD_PWM);
//    ES_Timer_InitTimer(BALL_DRIVER_TIMER, 500);
    ES_Timer_InitTimer(BALL_DRIVER_TIMER, 50);
}

uint8_t InitDeployerDriver(uint8_t priority) {
    //    printf("INIT DEPLOY\r\n");
    myPriority = priority;
    RC_SetPulseTime(RC_PORTX03, LOAD_PWM);
    currState = RESET;
    ES_Event thisEvent;
    thisEvent.EventType = ES_INIT;
    return ES_PostToService(myPriority, thisEvent);
}

uint8_t PostDeployerDriver(ES_Event thisEvent) {
    return ES_PostToService(myPriority, thisEvent);
}

ES_Event RunBallDispenser(ES_Event timerEvent) {
    ES_Event returnEvent;
    returnEvent.EventType = ES_NO_EVENT;

    if (timerEvent.EventType == ES_TIMEOUT) {
        //         printf("RUN\r\n");
        if (timerEvent.EventParam == BALL_DRIVER_TIMER) {

            switch (currState) {
                case LOAD:
                    //                    printf("LOAD\r\n");
                    IsTransmitEmpty();
                    RC_SetPulseTime(RC_PORTX03, LOAD_PWM);

                    currState = DISPENSE;
                    ES_Timer_InitTimer(BALL_DRIVER_TIMER, ACTION_PERIOD);

                    break;
                case DISPENSE:
                    //                    printf("DISPENSE\r\n");
                    IsTransmitEmpty();


                    RC_SetPulseTime(RC_PORTX03, DISPENSE_PWM);
                    currState = RESET;
                    ES_Timer_InitTimer(BALL_DRIVER_TIMER, ACTION_PERIOD);

                    break;
                case RESET:
                    //                    printf("RESET\r\n");
                    IsTransmitEmpty();
                    returnEvent.EventType = BALL_DEPLOYED;
                    returnEvent.EventParam = 0;
                    POST_DEPLOYER_EVENT(returnEvent);
                    RC_SetPulseTime(RC_PORTX03, LOAD_PWM);
                    isDispensing = 0;
                    break;
            }
        }
    }
    return returnEvent;
}


