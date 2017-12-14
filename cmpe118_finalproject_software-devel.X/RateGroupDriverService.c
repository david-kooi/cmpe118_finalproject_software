/*
 * File: TemplateService.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a simple service to work with the Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 13/Nov/2013
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#define ABS(x) ((x) < 0 ? (-(x)) : (x))

#include "BOARD.h"
#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "RateGroupDriverService.h"
#include "ES_Timers.h"
#include "TrackWireEventChecker.h"
#include "BeaconEventChecker.h"

#include "LsDerivative.h"

#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/
/* You will need MyPriority and maybe a state variable; you may need others
 * as well. */

static uint8_t MyPriority;
static uint8_t dtSamplingTrackWire;
static uint8_t sampleTw;
static Derivative dtTrackWire;
/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/
void DerivativeSampleTrackWire(void);

/**
 * @Function InitRateGroupService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateService function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitRateGroupDriverService(uint8_t Priority)
{

    dtTrackWire = newDerivative(2);
    sampleTw = 1;
    
    
    ES_Event ThisEvent;

    MyPriority = Priority;
    
    // Start all required timers
    ES_Timer_InitTimer(HZ_1_TIMER, 1000);
    ES_Timer_InitTimer(HZ_50_TIMER, 20);
    ES_Timer_InitTimer(HZ_500_TIMER, 2);

    dtSamplingTrackWire = FALSE;
       
    

    // post the initial transition event
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostRateGroupService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostRateGroupDriverService(ES_Event ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunRateGroupService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This service provides acts as a centra driver for various rate groups. 
 *        Max rate group resolution is 1ms(1000Hz)
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunRateGroupDriverService(ES_Event ThisEvent)
{
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors


    switch (ThisEvent.EventType) {
    case ES_INIT:
        //DEBUG_PRINT("ES_INIT");
        break;

                
    case ES_TIMEOUT:
        // Handle the various rate groups
        switch(ThisEvent.EventParam){
            
            
            
            
            case HZ_1_TIMER:
                //printf("1HZ TICK");
                
                
                
                // Restart Timer
                ES_Timer_InitTimer(HZ_1_TIMER, 1000); //1000 ms 
                break;
                
            case HZ_50_TIMER:
                //DEBUG_PRINT("20HZ TICK");
                TS_StartSampling();
                
                // Only check track wires when needed
                if(atm6KillCount != 3 && sampleTw){
                    RightTrackWireCheck();
                    LeftTrackWireCheck();
                }
                
                CheckBeacon();
                CheckBumpers();
                
                
                                
                // Restart Timer
                ES_Timer_InitTimer(HZ_50_TIMER, 20); //50 hz
                break;
   
            case HZ_500_TIMER:
                //printf("500HZ TICK");
                DerivativeSampleTrackWire();    

                
                
                
                
                // Restart Timer
                ES_Timer_InitTimer(HZ_500_TIMER, 2); //2 ms 
                break;
                
            case TS_SYNC_TIMER:
                //DEBUG_PRINT("TS_SYNC_TIMER TICK");
                TS_DriveSampling();
                
                break;

            default:

                break;
                

        }
        
        
        
        
        break;
    }


    return ReturnEvent;
}

uint8_t StartDerivative(){
    dtSamplingTrackWire = TRUE;
}

uint8_t StopDerivative(){
    dtSamplingTrackWire = FALSE;
}

uint8_t TwSampleOff(){
    sampleTw = 0;
}
uint8_t TwSampleOn(){
    sampleTw = 1;
}

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/
#define LOWER_VAL_THRESHOLD 30
#define LOWER_DT_THRESHOLD 100
#define UPPER_THRESHOLD 100
#define NUM_SAMPLES 10
#define MIN_VAL_THRESHOLD 300
void DerivativeSampleTrackWire(void){
    static uint8_t  isTracking = 0;
    static uint16_t counter = 0;
    static uint16_t twVal = 0;
    static int32_t  dtVal = 0;
    static uint16_t maxVal = 0;
    static ES_Event returnEvent;
    static uint16_t numBelow = 0;
    
    
    if(dtSamplingTrackWire == FALSE){
        return;
    }
//      val = TW_GetLeftReading();
//    if(val > UPPER_THRESHOLD){
//        ES_Event returnEvent;
//        returnEvent.EventType = TW_ZERO_DERIVATIVE;
//        returnEvent.EventParam = 0;
//        PostHsmTopLevel(returnEvent);
//        dtSamplingTrackWire = FALSE;
//    }
//    
    
        twVal = TW_GetLeftReading();

//            printf("%d\r\n", val);

    
    estimateDerivative(dtTrackWire, twVal);
    dtVal = getDerivative(dtTrackWire);
    if(ABS(dtVal) <  LOWER_DT_THRESHOLD && twVal > UPPER_THRESHOLD){
        ES_Event returnEvent;
        returnEvent.EventType = TW_ZERO_DERIVATIVE;
        returnEvent.EventParam = 0;
        
        PostHsmTopLevel(returnEvent);  
        dtSamplingTrackWire = FALSE;
    }
    
    
    
    counter++;
    
    if(counter >= NUM_SAMPLES){
       // printf("%d\r\n", dtVal);
//        ES_Event returnEvent;
//        returnEvent.EventType = TW_NULL_DERIVATIVE;
//        returnEvent.EventParam = 0;
//
//        
//        
//        if(val > 6000){ return; }
//        if(val > 300) { isTracking = 1; }
//        
////        if(val < LOWER_DT_THRESHOLD && isTracking){ 
//        
//        
//        if( (val < LOWER_DT_THRESHOLD) && (isTracking == 1)){ // The derivative is low enough
//            returnEvent.EventType = TW_ZERO_DERIVATIVE;            
//        }
//        
//        // Reset Variables
//        
//        isTracking = 0;
        counter = 0;
//        maxVal  = 0;
//        dtSamplingTrackWire = FALSE;
//        resetDerivative(dtTrackWire);
//        
//        
//        PostHsmTopLevel(returnEvent);       
    }
             
}
