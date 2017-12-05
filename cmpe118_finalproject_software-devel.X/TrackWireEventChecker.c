/*
 * File:   TrackWireEventChecker.c
 * Author: Talmor Kliman
 *
 * Created on November 27, 2017, 7:40 PM
 */

#include "TrackWireEventChecker.h"
#include <stdio.h>

uint16_t rightADCReading;
uint16_t leftADCReading;

void InitializeTrackWire(void){
    //AD_AddPins(TW_LEFT_ADC | TW_RIGHT_ADC);
}

uint8_t RightTrackWireCheck(void){
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    ES_EventTyp_t curEvent;
    
    rightADCReading = AD_ReadADPin(TW_RIGHT_ADC); //------------------------------------------------edit
    TWIRE_PRINT("RIGHT TWIRE: %d", rightADCReading);        
    
    if((rightADCReading < TRACKWIRE_IN_SIGHT_THRESHOLD)&&(leftADCReading < TRACKWIRE_IN_SIGHT_THRESHOLD)){ //check if the ATM6 is off to trigger a ATM6 Killed event below
       curEvent = TW_OFF;
    }
    
    if(rightADCReading >= TRACKWIRE_TOUCHING_THRESHOLD){
        TWIRE_PRINT("RIGHT_TOUCHING_THRESHOLD_REACHED %d", rightADCReading);
        curEvent = RIGHT_TRACKWIRE_TOUCHING_DETECTED;
    } else if(rightADCReading >= TRACKWIRE_IN_SIGHT_THRESHOLD){
         TWIRE_PRINT("RIGHT_IN_SIGHT_THRESHOLD_REACHED %d", rightADCReading);
        curEvent = TW_RIGHT_IN_SIGHT;
    } else if((lastEvent == TW_RIGHT_IN_SIGHT || lastEvent == TW_RIGHT_TOUCHING) //ATM6 Killed Event based on reading above 
             && curEvent == TW_OFF){  
         TWIRE_PRINT("LEFT_ATM6_KILLED%d", leftADCReading);
         curEvent = TW_OFF;          
    } else {
        curEvent = ES_NO_EVENT;
    }

     if (curEvent != lastEvent) { // check for change from last time
            ReturnEvent.EventType = curEvent;
            ReturnEvent.EventParam = rightADCReading;
            lastEvent = curEvent; // update history
            //TWIRE_PRINT("RIGHT_TW_EVENT, EventType: %p, EventParam %u",ReturnEvent.EventType,ReturnEvent.EventParam);
            
#ifdef TEST_HARNESS_SERVICE_TEST
            PostTestHarnessService(ReturnEvent); //-------------------------------------------------------edit
#else
            PostHsmTopLevel(ReturnEvent);
#endif
            
        }
        
        return 0;

}

uint8_t LeftTrackWireCheck(void){
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    ES_EventTyp_t curEvent;
    
    uint16_t leftADCReading = AD_ReadADPin(TW_LEFT_ADC); //------------------------------------------------edit
    TWIRE_PRINT("LEFT TWIRE: %d", leftADCReading);        
        
    if((leftADCReading < TRACKWIRE_IN_SIGHT_THRESHOLD)&&(rightADCReading < TRACKWIRE_IN_SIGHT_THRESHOLD)){ //check if the ATM6 is off to trigger a ATM6 Killed event below
        curEvent = TW_OFF;
    }
    
    if(leftADCReading >= TRACKWIRE_TOUCHING_THRESHOLD){
        TWIRE_PRINT("LEFT_TOUCHING_THRESHOLD_REACHED %d", leftADCReading);
        curEvent = TW_LEFT_TOUCHING;
    } else if(leftADCReading >= TRACKWIRE_IN_SIGHT_THRESHOLD){
         TWIRE_PRINT("LEFT_IN_SIGHT_THRESHOLD_REACHED %d", leftADCReading);
         curEvent = TW_LEFT_IN_SIGHT;
    }else if((lastEvent == TW_LEFT_IN_SIGHT || lastEvent == TW_LEFT_TOUCHING) //ATM6 Killed Event based on reading above 
             && curEvent == TW_OFF){  
         TWIRE_PRINT("LEFT_ATM6_KILLED%d", leftADCReading);
         curEvent = TW_OFF;            
    }else {
        curEvent = ES_NO_EVENT;
    }
    
     if (curEvent != lastEvent) { // check for change from last time
            ReturnEvent.EventType = curEvent;
            ReturnEvent.EventParam = leftADCReading;
            lastEvent = curEvent; // update history
            //TWIRE_PRINT("LEFT_TW_EVENT, EventType: %p, EventParam %u",ReturnEvent.EventType,ReturnEvent.EventParam);

#ifdef TEST_HARNESS_SERVICE_TEST
            PostTestHarnessService(ReturnEvent); //-------------------------------------------------------edit
#else
            PostHsmTopLevel(ReturnEvent);
#endif
        }
    
    return 0;
}
