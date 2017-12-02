/*
 * File:   TrackWireEventChecker.c
 * Author: Talmor Kliman
 *
 * Created on November 27, 2017, 7:40 PM
 */

#include "TrackWireEventChecker.h"
#include <stdio.h>

#define TW_LEFT_ADC  AD_PORTV6
#define TW_RIGHT_ADC AD_PORTV4


void InitializeTrackWire(void){
    AD_AddPins(TW_LEFT_ADC);
    AD_AddPins(TW_RIGHT_ADC);
}

uint8_t RightTrackWireCheck(void){
    ES_Event ReturnEvent;
    ReturnEvent.EventType = TRACKWIRE_NOT_DETECTED; // assume no errors

    static ES_EventTyp_t lastEvent = TRACKWIRE_NOT_DETECTED;
    ES_EventTyp_t curEvent;
    
    uint16_t rightADCReading = AD_ReadADPin(TW_RIGHT_ADC); //------------------------------------------------edit
    TWIRE_PRINT("RIGHT TWIRE: %d", rightADCReading);        
    
    
    if(rightADCReading >= TRACKWIRE_TOUCHING_THRESHOLD){
        TWIRE_PRINT("RIGHT_TOUCHING_THRESHOLD_REACHED %d", rightADCReading);
        curEvent = RIGHT_TRACKWIRE_TOUCHING_DETECTED;
    } else if(rightADCReading >= TRACKWIRE_IN_SIGHT_THRESHOLD){
         TWIRE_PRINT("RIGHT_IN_SIGHT_THRESHOLD_REACHED %d", rightADCReading);
        curEvent = RIGHT_TRACKWIRE_DETECTED;
    } else {
        curEvent = TRACKWIRE_NOT_DETECTED;
    }
    
     if (curEvent != lastEvent) { // check for change from last time
            ReturnEvent.EventType = curEvent;
            ReturnEvent.EventParam = rightADCReading;
            lastEvent = curEvent; // update history
            //TWIRE_PRINT("RIGHT_TW_EVENT, EventType: %p, EventParam %u",ReturnEvent.EventType,ReturnEvent.EventParam);
            
#ifdef TEST_HARNESS_SERVICE_TEST
            PostTestHarnessService(ReturnEvent); //-------------------------------------------------------edit
#else
            //PostHSM(ReturnEvent);
#endif
            
        }
        
        return 0;

}

uint8_t LeftTrackWireCheck(void){
    ES_Event ReturnEvent;
    ReturnEvent.EventType = TRACKWIRE_NOT_DETECTED; // assume no errors

    static ES_EventTyp_t lastEvent = TRACKWIRE_NOT_DETECTED;
    ES_EventTyp_t curEvent;
    
    uint16_t leftADCReading = AD_ReadADPin(TW_LEFT_ADC); //------------------------------------------------edit
    TWIRE_PRINT("LEFT TWIRE: %d", leftADCReading);        
        
    
    if(leftADCReading >= TRACKWIRE_TOUCHING_THRESHOLD){
        TWIRE_PRINT("LEFT_TOUCHING_THRESHOLD_REACHED %d", leftADCReading);
        curEvent = LEFT_TRACKWIRE_TOUCHING_DETECTED;
    } else if(leftADCReading >= TRACKWIRE_IN_SIGHT_THRESHOLD){
         TWIRE_PRINT("LEFT_IN_SIGHT_THRESHOLD_REACHED %d", leftADCReading);
         curEvent = LEFT_TRACKWIRE_DETECTED;
    } else {
        curEvent = TRACKWIRE_NOT_DETECTED;
    }
    
     if (curEvent != lastEvent) { // check for change from last time
            ReturnEvent.EventType = curEvent;
            ReturnEvent.EventParam = leftADCReading;
            lastEvent = curEvent; // update history
            //TWIRE_PRINT("LEFT_TW_EVENT, EventType: %p, EventParam %u",ReturnEvent.EventType,ReturnEvent.EventParam);

#ifdef TEST_HARNESS_SERVICE_TEST
            PostTestHarnessService(ReturnEvent); //-------------------------------------------------------edit
#else
            //PostHSM(ReturnEvent);
#endif
        }
    
    return 0;
}
