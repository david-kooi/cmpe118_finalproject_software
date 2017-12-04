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

typedef enum {
    TS_RIGHT_SAMPLE,
    TS_LEFT_SAMPLE,
    TS_CENTER_SAMPLE,
    TS_REAR_SAMPLE,
    TS_CHECK_SAMPLES
} TS_State;


// Sampling Time
#define TS_SAMPLE_TIME 1 // ms

#define TS_PORT PORTZ

// Tape Sensor Port Numbers
#define TS_LEFT_TRIG_PIN      PIN9
#define TS_LEFT_ADC           AD_PORTW8
#define TS_LEFT_LED_PIN       PIN10
 
#define TS_RIGHT_TRIG_PIN      PIN7
#define TS_RIGHT_ADC           AD_PORTW7
#define TS_RIGHT_LED_PIN       PIN8

#define TS_CENTER_TRIG_PIN      PIN5
#define TS_CENTER_ADC           AD_PORTW5
#define TS_CENTER_LED_PIN       PIN6
 
#define TS_REAR_TRIG_PIN      PIN3
#define TS_REAR_ADC           AD_PORTW3
#define TS_REAR_LED_PIN       PIN4


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
uint16_t CheckTapeSensor(char* desc, uint8_t adcPin);

void HandleTapeSensorEvent(uint8_t prevVal, uint8_t currVal, 
                            uint8_t ON_EVENT, uint8_t OFF_EVENT,
                            ES_Event thisEvent){

 
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
        IO_PortsSetPortOutputs(TS_PORT, TS_LEFT_TRIG_PIN);
        IO_SOURCE(TS_PORT, TS_LEFT_TRIG_PIN); // Start emitter off
        // ADC Pin
        AD_AddPins(TS_LEFT_ADC);
        // LED Indicator
        IO_PortsSetPortOutputs(TS_PORT, TS_LEFT_LED_PIN);
        IO_SOURCE(TS_PORT, TS_LEFT_LED_PIN); // Set indicator off
        
    /* RIGHT SENSOR*/
        
        // Trigger Pin
        IO_PortsSetPortOutputs(TS_PORT, TS_RIGHT_TRIG_PIN);
        IO_SOURCE(TS_PORT, TS_RIGHT_TRIG_PIN); // Start emitter off
        // ADC Pin
        AD_AddPins(TS_RIGHT_ADC);
        // LED Indicator
        IO_PortsSetPortOutputs(TS_PORT, TS_RIGHT_LED_PIN);
        IO_SOURCE(TS_PORT, TS_RIGHT_LED_PIN); // Set indicator off
    
  
    
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
    TS_EMITTER_STATE      = TS_RIGHT_SAMPLE;
    
    // Start sampling timer to begin sampling
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
    static uint16_t firstRun                = 1; 
    
    // Sensor Variables
    static uint16_t leftSensorVal[2]        = {0};
    static uint16_t rightSensorVal[2]       = {0};
    static uint16_t centerSensorVal[2]      = {0};
    static uint16_t rearSensorVal[2]        = {0};


    
    ES_Event thisEvent;
    thisEvent.EventType = ES_NO_EVENT;
    
    

    // Continue if we are in initial sampling state
    switch(TS_EMITTER_STATE){
        
        case TS_RIGHT_SAMPLE:
            TAPE_PRINT("TS RIGHT SAMPLE");            
                   
            IO_SINK(TS_LEFT_TRIG_PIN); // Turn emitter on
            leftSensorVal[TS_VAL_CURR] = CheckTapeSensor_LEFT("RIGHT", TS_RIGHT_ADC);
                
                
            // Change State and Restart Sensor
            TS_EMITTER_STATE = TS_LEFT_SAMPLE;
            ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // ms Tape Sensor Sampling    

            break;
                    
        case TS_LEFT_SAMPLE:
            TAPE_PRINT("TS LEFT SAMPLE");
                
            IO_SOURCE(TS_RIGHT_TRIG_PIN); // Turn previous emitter off
                
            IO_SINK(TS_LEFT_TRIG_PIN); // Turn emitter on
            leftSensorVal[TS_VAL_CURR] = CheckTapeSensor_LEFT("LEFT", TS_LEFT_ADC);
                
            // Change State and Restart Sensor
            TS_EMITTER_STATE = TS_CENTER_SAMPLE;
            ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // ms Tape Sensor Sampling 

            break;
        case TS_CENTER_SAMPLE:
            TAPE_PRINT("TS CENTER SAMPLE");

            IO_SOURCE(TS_LEFT_TRIG_PIN); // Turn previous emitter off

            IO_SINK(TS_CENTER_TRIG_PIN); // Turn emitter on
            centerSensorVal[TS_VAL_CURR] = CheckTapeSensor_CENTER("CENTER", TS_CENTER_ADC);

            // Change State and Restart Sensor
            TS_EMITTER_STATE = TS_REAR_SAMPLE;
            ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // ms Tape Sensor Sampling  

            break;
                
        case TS_REAR_SAMPLE:
            TAPE_PRINT("TS REAR SAMPLE");

            IO_SOURCE(TS_CENTER_TRIG_PIN); // Turn previous emitter off
            rearSensorVal[TS_VAL_CURR] = CheckTapeSensor_REAR("REAR", TS_REAR_ADC);

            // Change State and Restart Sensor
            TS_EMITTER_STATE = TS_CHECK_SAMPLES;
            ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // ms Tape Sensor Sampling  

            break;

        case TS_CHECK_SAMPLES:
            // Check Left
            HandleTapeSensorEvent(firstRun, leftSensorVal[TS_VAL_PREV], leftSensorVal[TS_VAL_CURR],
                                  TS_LEFT_ON, TS_LEFT_OFF, thisEvent);
            // Check Right
            HandleTapeSensorEvent(firstRun, rightSensorVal[TS_VAL_PREV], rightSensorVal[TS_VAL_CURR],
                                  TS_RIGHT_ON, TS_RIGHT_OFF, thisEvent);
            // Check Center
            HandleTapeSensorEvent(firstRun, centerSensorVal[TS_VAL_PREV], centerSensorVal[TS_VAL_CURR],
                                  TS_CENTER_ON, TS_CENTER_OFF, thisEvent);
            // Check Rear
            HandleTapeSensorEvent(firstRun, rearSensorVal[TS_VAL_PREV], rearSensorVal[TS_VAL_CURR],
                                  TS_REAR_ON, TS_REAR_OFF, thisEvent);


            if(firstRun){ // First run initializes previous values
                firstRun = 0;
            }

            // We are finished sampling
            TS_SAMPLING_ACTIVE = 0;
            break;
                
        default:

            break;
        
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
void HandleTapeSensorEvent(uint8_t firstRun, uint8_t prevVal, uint8_t currVal, 
                            uint8_t ledPin, uint8_t ON_EVENT, uint8_t OFF_EVENT,
                            uint16_t lowerThresh, uint16_t upperThresh,
                            ES_Event thisEvent){
    if(firstRun){
        prevVal = currVal;
        return;
    }

    // On tape event
    if      (prevVal < upperThresh && currVal > upperThresh){
        thisEvent.EventType = ON_EVENT;
        IO_SINK(TS_PORT, ledPin); //Turn LED ON
    }
    // Off tape event
    else if(prevVal > lowerThresh && currVal < lowerThresh){
        thisEvent.EventType = OFF_EVENT;
        IO_SOURCE(TS_PORT, ledPin); // Turn LED OFF
    }

    // Set param
    thisEvent.EventParam = sensorVal;
   // Post
    PostHsmTopLevel(thisEvent);
                
}

/**
 * @Function CheckTapeSensor_LEFT(void)
 * @param none
 * @return TRUE or FALSE
 * @brief ...
 * @note 
 * @author David Kooi, 2017.11.21
 * @modified */
uint16_t CheckTapeSensor(char* desc, uint8_t adcPin){
    uint16_t val = 0;
    val = AD_ReadADPin(pin);
    TAPE_PRINT("TAPE SENSOR %s: %d\r\n", desc, val);
    
    return val;
}