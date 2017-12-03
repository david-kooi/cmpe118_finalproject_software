#include "TapeSensorEventChecker.h"

#include <stdlib.h>

/*******************************************************************************
 * MODULE Variables                                                            *
 ******************************************************************************/
// Sensor States
uint8_t sensorState_CURR = 0x00;
uint8_t sensorState_PREV = 0x00;

// Sampling States
 uint8_t TS_EMITTER_STATE = 0;

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define TS_VAL_CURR 0
#define TS_VAL_PREV 1
 
#define TS_EMITTER_ON   1
#define TS_EMITTER_OFF  2
 

// Sampling Time
#define TS_SAMPLE_TIME 2 // ms

// Tape Sensor Port Numbers
#define TS_LEFT_TRIG_NUM      PORTZ, PIN9
#define TS_LEFT_ADC           AD_PORTW8
#define TS_LEFT_LED_NUM       PORTZ, PIN10
 
#define TS_RIGHT_TRIG_NUM      PORTZ, PIN7
#define TS_RIGHT_ADC           AD_PORTW7
#define TS_RIGHT_LED_NUM       PORTZ, PIN8

#define TS_CENTER_TRIG_NUM      PORTZ, PIN5
#define TS_CENTER_ADC           AD_PORTW5
#define TS_CENTER_LED_NUM       PORTZ, PIN6
 
#define TS_REAR_TRIG_NUM      PORTZ, PIN3
#define TS_REAR_ADC           AD_PORTW3
#define TS_REAR_LED_NUM       PORTZ, PIN4


// Sensor Thresholds
#define TS_LEFT_HI_THRESH 900
#define TS_LEFT_LO_THRESH 700

#define TS_RIGHT_HI_THRESH 900
#define TS_RIGHT_LO_THRESH 700
 
#define TS_CENTER_HI_THRESH 900
#define TS_CENTER_LO_THRESH 700

#define TS_REAR_HI_THRESH 900
#define TS_REAR_LO_THRESH 700

 // Forward Declared Functions
 uint16_t CheckTapeSensor_LEFT(void);
 uint16_t CheckTapeSensor_RIGHT(void);
 
 void HandleTapeSensorEvent(uint8_t prevState, uint8_t currState, 
                            uint8_t ON_EVENT, uint8_t OFF_EVENT,
                            uint16_t sensorVal, ES_Event thisEvent);

 
/**
 * @Function InitializeTapeSensors(void)
 * @param none
 * @return TRUE or FALSE
 * @brief ...
 * @note 
 * @author David Kooi, 2017.11.21
 * @modified */
uint8_t InitializeTapeSensors(void){
    
    // Sensors start as sourcing. (Photo Emitter ) 
    
    /* LEFT SENSOR */
    
        // Trigger Pin
        IO_PortsSetPortOutputs(TS_LEFT_TRIG_NUM);
        IO_SOURCE(TS_LEFT_TRIG_NUM); // Start emitter off
        // ADC Pin
        AD_AddPins(TS_LEFT_ADC);
        // LED Indicator
        IO_PortsSetPortOutputs(TS_LEFT_LED_NUM);
        IO_SOURCE(TS_LEFT_LED_NUM); // Set indicator off
        
    /* RIGHT SENSOR*/
        
        // Trigger Pin
        IO_PortsSetPortOutputs(TS_RIGHT_TRIG_NUM);
        IO_SOURCE(TS_RIGHT_TRIG_NUM); // Start emitter off
        // ADC Pin
        AD_AddPins(TS_RIGHT_ADC);
        // LED Indicator
        IO_PortsSetPortOutputs(TS_RIGHT_LED_NUM);
        IO_SOURCE(TS_RIGHT_LED_NUM); // Set indicator off
    
  
    
}

/**
 * @Function TS_StartSampling(void)
 * @param none
 * @return TRUE if an event occured or FALSE if no event occured
 * @brief Start asynchronously sync-sampling the tape sensors 
 * @note Subsequent calls to StartPing() will have no effect until the initial
   ping is complete.
 * @author David Kooi, 2017.11.21
 * @modified */
void TS_StartSampling(void){
    if(TS_SAMPLING_ACTIVE){
        return;
    }
    TAPE_PRINT("TS Started Sampling");
        
    // Initialize for synchronous sampling
    TS_SAMPLING_ACTIVE    = 1;
    TS_EMITTER_STATE      = TS_EMITTER_ON;
    
    // Start sampling timer
    ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // 2ms Tape Sensor Sampling

}

/**
 * @Function TS_DriveSampling(void)
 * @param none
 * @return 
 * @brief 
 * @note If a change occurs an event is emitted
 * @author David Kooi, 2017.11.21
 * @modified */
void TS_DriveSampling(void){
    static uint16_t tmp_1                = 0;
    static uint16_t currState, prevState = 0;
    
    // Sensor Variables
    static uint16_t leftSensorVal        = 0;
    static uint16_t rightSensorVal       = 0;
    static uint16_t centerSensorVal      = 0;
    static uint16_t rearSensorVal        = 0;
    
    ES_Event thisEvent;
    thisEvent.EventType = ES_NO_EVENT;
    
    

    // Continue if we are in initial sampling state
    switch(TS_EMITTER_STATE){
        
        case TS_EMITTER_ON:
            TAPE_PRINT("TS DRIVER STATE: EMITTER ON");

            // Sample all sensors with their emitters on
            
           /* LEFT SENSOR */        
                //IO_SINK(TS_LEFT_TRIG_NUM); // Turn emitter on
                leftSensorVal = CheckTapeSensor_LEFT();
                IO_SOURCE(TS_LEFT_TRIG_NUM); // Turn emitter off
                

            /* RIGHT SENSOR */
                IO_SINK(TS_RIGHT_TRIG_NUM); // Turn emitter on
                rightSensorVal = CheckTapeSensor_RIGHT();
                //IO_SOURCE(TS_RIGHT_TRIG_NUM); // Turn emitter off
                
            
            // Change State and Restart Sensor
            TS_EMITTER_STATE = TS_EMITTER_OFF;
            ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // ms Tape Sensor Sampling            
            break;
                    
        case TS_EMITTER_OFF:
            TAPE_PRINT("TS DRIVER STATE: EMITTER OFF");
                
            
                // Sample all sensors with their emitters off
                // Emitters are turned off after TS_EMITTER_ON sampling
                
                /* LEFT SENSOR */
                    tmp_1 = CheckTapeSensor_LEFT();
                    leftSensorVal = abs((int32_t)tmp_1 - leftSensorVal);
                    //TAPE_PRINT("DELTA: %d", leftSensorVal);
                    if     (leftSensorVal > TS_LEFT_HI_THRESH){sensorState_CURR |= TS_LEFT_SENSOR;}  // Toggle sensor bit 
                    else if(leftSensorVal < TS_LEFT_LO_THRESH){sensorState_CURR &= ~TS_LEFT_SENSOR;} // Untoggle sensor bit 
                    
                    // Check for change
                    prevState = sensorState_PREV & TS_LEFT_SENSOR; // previous state
                    currState = sensorState_CURR & TS_LEFT_SENSOR; // current state
                    
                    HandleTapeSensorEvent(prevState, currState, 
                                          TS_LEFT_ON, TS_LEFT_OFF,
                                          leftSensorVal, thisEvent);
                    
                    
                /* RIGHT SENSOR */
 
                    tmp_1 = CheckTapeSensor_RIGHT();
                    rightSensorVal = abs((int32_t)tmp_1 - rightSensorVal);
                    //TAPE_PRINT("DELTA: %d", rightSensorVal);
                    if     (rightSensorVal > TS_RIGHT_HI_THRESH){sensorState_CURR |= TS_RIGHT_SENSOR;}  // Toggle sensor bit 
                    else if(rightSensorVal < TS_RIGHT_LO_THRESH){sensorState_CURR &= ~TS_RIGHT_SENSOR;} // Untoggle sensor bit 
                    
                    // Check for change
                    prevState = sensorState_PREV & TS_RIGHT_SENSOR; // previous state
                    currState = sensorState_CURR & TS_RIGHT_SENSOR; // current state
                    
                    // Check for change
                    prevState = sensorState_PREV & TS_LEFT_SENSOR; // previous state
                    currState = sensorState_CURR & TS_LEFT_SENSOR; // current state
                    
                    // Handle Change
                    HandleTapeSensorEvent(prevState, currState, 
                                          TS_RIGHT_ON, TS_RIGHT_OFF,
                                          rightSensorVal, thisEvent);
                                 

                    
                    
                    
                    
                    
                // Sampling finished
                sensorState_PREV = sensorState_CURR;
                
                TS_SAMPLING_ACTIVE = 0;
                break;
                
                default:
                    break;
        
    }
    

    
    
    
    
    // If the emitter is off we know we are finished sampling
    if(TS_EMITTER_ON == 0){
        
    }
    
}


/**
 * @Function HandleTapeSensorEvent
 * @param Needed parameters
 * @return void
 * @brief Handles a tape sensor event
 * @note 
 * @author David Kooi, 2017.11.21
 * @modified */
void HandleTapeSensorEvent(uint8_t prevState, uint8_t currState, 
                            uint8_t ON_EVENT, uint8_t OFF_EVENT,
                            uint16_t sensorVal, ES_Event thisEvent){

    if( prevState ^ currState ){  // There has been a change

    // If current state is greater than zero the tape sensor has turned on
    (currState > 0) ? (thisEvent.EventType = ON_EVENT) : (thisEvent.EventType = OFF_EVENT);

    // Toggle LED if the current state is greater than 0
    (currState > 0) ? IO_SINK(TS_LEFT_LED_NUM) : IO_SOURCE(TS_LEFT_LED_NUM);

    // Set param
    thisEvent.EventParam = sensorVal;

    // Post to services
#ifdef TEST_HARNESS_SERVICE_TEST
    PostTestHarnessService(thisEvent);
#else
    //PostHSM(thisEvent);
#endif
                        
                        
                    }
}

/**
 * @Function CheckTapeSensor_LEFT(void)
 * @param none
 * @return TRUE or FALSE
 * @brief ...
 * @note 
 * @author David Kooi, 2017.11.21
 * @modified */
uint16_t CheckTapeSensor_LEFT(void){
    uint16_t val = 0;
    val = AD_ReadADPin(TS_LEFT_ADC);
    TAPE_PRINT("TAPE SENSOR LEFT: %d\r\n", val);
    
    return val;
}

/**
 * @Function CheckTapeSensor_RIGHT(void)
 * @param none
 * @return TRUE or FALSE
 * @brief ...
 * @note 
 * @author David Kooi, 2017.11.21
 * @modified */
uint16_t CheckTapeSensor_RIGHT(void){
    uint16_t val = 0;
    val = AD_ReadADPin(TS_RIGHT_ADC);
    TAPE_PRINT("TAPE SENSOR RIGHT: %d\r\n", val);
    
    return val;
}