#include "BeaconEventChecker.h"

#include <stdlib.h>

/*******************************************************************************
 * MODULE Variables                                                            *
 ******************************************************************************/


/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

void CheckBeacon(void){

    uint16_t val =  AD_ReadADPin(BEACON_ADC);
    BEACON_PRINT("BEACON: %d\r\n", val);
    
    ES_Event returnEvent;
    static ES_Event prevEvent;
    
        if( val > HI_IN_SIGHT_THRESH){
            returnEvent.EventType = BC_IN_SIGHT;
        }
        
        if(val > HI_HEAD_ON_THRESH){
            returnEvent.EventType = BC_HEAD_ON;
        }
    
        if(val < LO_HEAD_ON_THRESH){
            returnEvent.EventType = BC_IN_SIGHT;
        }
    
        if(val < LO_IN_SIGHT_THRESH){
            returnEvent.EventType = BC_OFF;
        }
    
        
    
    
    if(returnEvent.EventType != prevEvent.EventType){
        PostHsmTopLevel(returnEvent);
        prevEvent.EventType = returnEvent.EventType;
    }
    
    
    
}


 
	

