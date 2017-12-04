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

enum {
    TS_VAL_CURR,
    TS_VAL_PREV
};

static uint8_t TS_SAMPLING_ACTIVE = 0;



 // Forward Declared Functions
uint16_t CheckTapeSensor(char* desc, uint32_t adcPin);

void HandleTapeSensorEvent(uint8_t firstRun, uint32_t prevVal, uint32_t currVal,  
                            uint32_t ledPin, uint16_t ON_TAPE_EVENT, uint16_t OFF_TAPE_EVENT,
                            uint32_t lowerThresh, uint32_t upperThresh,
                            ES_Event thisEvent);

 
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
    
    //AD_AddPins(TS_LEFT_ADC | TS_RIGHT_ADC | TS_CENTER_ADC | TS_REAR_ADC);

    /* LEFT SENSOR */
    
        // Trigger Pin
        IO_PortsSetPortOutputs(TS_PORT, TS_LEFT_TRIG_PIN);
        IO_SOURCE(TS_PORT, TS_LEFT_TRIG_PIN); // Start emitter off
        // LED Indicator
        IO_PortsSetPortOutputs(PORTX, TS_LEFT_LED_PIN);
        IO_SOURCE(PORTX, TS_LEFT_LED_PIN); // Set indicator off
        
    /* RIGHT SENSOR*/
        
        // Trigger Pin
        IO_PortsSetPortOutputs(TS_PORT, TS_RIGHT_TRIG_PIN);
        IO_SOURCE(TS_PORT, TS_RIGHT_TRIG_PIN); // Start emitter off
        // LED Indicator
        IO_PortsSetPortOutputs(TS_PORT, TS_RIGHT_LED_PIN);
        IO_SOURCE(TS_PORT, TS_RIGHT_LED_PIN); // Set indicator off
    
    /* CENTER SENSOR*/
        
        // Trigger Pin
        IO_PortsSetPortOutputs(TS_PORT, TS_CENTER_TRIG_PIN);
        IO_SOURCE(TS_PORT, TS_CENTER_TRIG_PIN); // Start emitter off
        // LED Indicator
        IO_PortsSetPortOutputs(TS_PORT, TS_CENTER_LED_PIN);
        IO_SOURCE(TS_PORT, TS_CENTER_LED_PIN); // Set indicator off
        
    /* REAR SENSOR*/
        
        // Trigger Pin
        IO_PortsSetPortOutputs(TS_PORT, TS_REAR_TRIG_PIN);
        IO_SOURCE(TS_PORT, TS_REAR_TRIG_PIN); // Start emitter off
        // LED Indicator
        IO_PortsSetPortOutputs(TS_PORT, TS_REAR_LED_PIN);
        IO_SOURCE(TS_PORT, TS_REAR_LED_PIN); // Set indicator off
        
        
    
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
    
    IO_SINK(TS_PORT, TS_RIGHT_TRIG_PIN); // Turn emitter on
    
    // Start sampling timer to begin sampling
    ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // ms Tape Sensor Sampling

    
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
    static uint32_t leftSensorVal[2]        = {0};
    static uint32_t rightSensorVal[2]       = {0};
    static uint32_t centerSensorVal[2]      = {0};
    static uint32_t rearSensorVal[2]        = {0};


    
    ES_Event thisEvent;
    thisEvent.EventType = ES_NO_EVENT;
    
    

    // Continue if we are in initial sampling state
    switch(TS_EMITTER_STATE){
        
        case TS_RIGHT_SAMPLE:
            //TAPE_PRINT("TS RIGHT SAMPLE");            
                   
            rightSensorVal[TS_VAL_CURR] = CheckTapeSensor("RIGHT", TS_RIGHT_ADC);
            IO_SOURCE(TS_PORT, TS_RIGHT_TRIG_PIN); // Turn emitter off
            IO_SINK(TS_PORT, TS_LEFT_TRIG_PIN); // Turn next emitter on
                
            // Change State and Restart Sensor
            TS_EMITTER_STATE = TS_LEFT_SAMPLE;
            ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // ms Tape Sensor Sampling    

            break;
                    
        case TS_LEFT_SAMPLE:
            //TAPE_PRINT("TS LEFT SAMPLE");
                
            leftSensorVal[TS_VAL_CURR] = CheckTapeSensor("LEFT", TS_LEFT_ADC);
            IO_SOURCE(TS_PORT, TS_LEFT_TRIG_PIN); // Turn emitter off
            IO_SINK(TS_PORT, TS_CENTER_TRIG_PIN); // Turn emitter on
            
            // Change State and Restart Sensor
            TS_EMITTER_STATE = TS_CENTER_SAMPLE;
            ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // ms Tape Sensor Sampling 

            break;
        case TS_CENTER_SAMPLE:
            //TAPE_PRINT("TS CENTER SAMPLE");

            centerSensorVal[TS_VAL_CURR] = CheckTapeSensor("CENTER", TS_CENTER_ADC);
            IO_SOURCE(TS_PORT, TS_CENTER_TRIG_PIN); // Turn previous emitter off
            IO_SINK(TS_PORT, TS_REAR_TRIG_PIN);

            // Change State and Restart Sensor
            TS_EMITTER_STATE = TS_REAR_SAMPLE;
            ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // ms Tape Sensor Sampling  

            break;
                
        case TS_REAR_SAMPLE:
            //TAPE_PRINT("TS REAR SAMPLE");
            rearSensorVal[TS_VAL_CURR] = CheckTapeSensor("REAR", TS_REAR_ADC);
            IO_SOURCE(TS_PORT, TS_REAR_TRIG_PIN);

            // Change State and Restart Sensor
            TS_EMITTER_STATE = TS_CHECK_SAMPLES;
            ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // ms Tape Sensor Sampling  

            break;

        case TS_CHECK_SAMPLES:
            
            /*
            void HandleTapeSensorEvent(uint8_t firstRun, uint8_t prevVal, uint8_t currVal, 
                            uint8_t ledPin, uint8_t ON_EVENT, uint8_t OFF_EVENT,
                            uint32_t lowerThresh, uint32_t upperThresh,
                            ES_Event thisEvent){
            */
            
            TAPE_PRINT("TAPE SENSOR RIGHT: %u\r\n",  rightSensorVal[TS_VAL_CURR]);
            TAPE_PRINT("TAPE SENSOR CENTER: %u\r\n", centerSensorVal[TS_VAL_CURR]);
            TAPE_PRINT("TAPE SENSOR LEFT: %u\r\n",   leftSensorVal[TS_VAL_CURR]);
            TAPE_PRINT("TAPE SENSOR REAR: %u\r\n",   rearSensorVal[TS_VAL_CURR]);
            
            
            if(firstRun){
                leftSensorVal[TS_VAL_PREV]   = leftSensorVal[TS_VAL_CURR];
                rightSensorVal[TS_VAL_PREV]  = rightSensorVal[TS_VAL_CURR];
                centerSensorVal[TS_VAL_PREV] = centerSensorVal[TS_VAL_CURR];
                rearSensorVal[TS_VAL_PREV]   = rearSensorVal[TS_VAL_CURR];

                firstRun = 0;
            }
            
            // Check Left
            HandleTapeSensorEvent(firstRun, leftSensorVal[TS_VAL_PREV], leftSensorVal[TS_VAL_CURR],
                                  TS_LEFT_LED_PIN, TS_LEFT_ON_TAPE, TS_LEFT_OFF_TAPE, 
                                  TS_LEFT_LO_THRESH, TS_LEFT_HI_THRESH, thisEvent);
            // Check Right
            HandleTapeSensorEvent(firstRun, rightSensorVal[TS_VAL_PREV], rightSensorVal[TS_VAL_CURR],
                                  TS_RIGHT_LED_PIN, TS_RIGHT_ON_TAPE, TS_RIGHT_OFF_TAPE, 
                                  TS_RIGHT_LO_THRESH, TS_RIGHT_HI_THRESH, thisEvent);
            // Check Center
            HandleTapeSensorEvent(firstRun, centerSensorVal[TS_VAL_PREV], centerSensorVal[TS_VAL_CURR],
                                  TS_CENTER_LED_PIN, TS_CENTER_ON_TAPE, TS_CENTER_OFF_TAPE, 
                                  TS_CENTER_LO_THRESH, TS_CENTER_HI_THRESH, thisEvent);
            // Check Rear
            HandleTapeSensorEvent(firstRun, rearSensorVal[TS_VAL_PREV], rearSensorVal[TS_VAL_CURR],
                                  TS_REAR_LED_PIN, TS_REAR_ON_TAPE, TS_REAR_OFF_TAPE,
                                  TS_REAR_LO_THRESH, TS_REAR_HI_THRESH, thisEvent);
            
            // Set prev values
            leftSensorVal[TS_VAL_PREV]   = leftSensorVal[TS_VAL_CURR];
            rightSensorVal[TS_VAL_PREV]  = rightSensorVal[TS_VAL_CURR];
            centerSensorVal[TS_VAL_PREV] = centerSensorVal[TS_VAL_CURR];
            rearSensorVal[TS_VAL_PREV]   = rearSensorVal[TS_VAL_CURR];

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
void HandleTapeSensorEvent(uint8_t firstRun, uint32_t prevVal, uint32_t currVal, 
                            uint32_t ledPin, uint16_t ON_TAPE_EVENT, uint16_t OFF_TAPE_EVENT,
                            uint32_t lowerThresh, uint32_t upperThresh,
                            ES_Event thisEvent){

    static uint8_t port = 0;
    if(ON_TAPE_EVENT == TS_LEFT_ON_TAPE || OFF_TAPE_EVENT == TS_LEFT_OFF_TAPE){
        port = PORTX;
    }else{
        port = TS_PORT;
    }
    
    // Off tape event
    if      (prevVal < upperThresh && currVal > upperThresh){
        thisEvent.EventType = OFF_TAPE_EVENT;
        IO_SOURCE(port, ledPin); //Turn LED OFF
    }
    // On tape event
    else if(prevVal > lowerThresh && currVal < lowerThresh){
        thisEvent.EventType = ON_TAPE_EVENT;
        IO_SINK(port, ledPin); // Turn LED ON
    }else{
        // Nothing to do
        return;
    }

    // Set param
    thisEvent.EventParam = currVal;
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
uint16_t CheckTapeSensor(char* desc, uint32_t adcPin){
    uint16_t val = 0;
    val = AD_ReadADPin(adcPin);
    //TAPE_PRINT("TAPE SENSOR %s: %d\r\n", desc, val);
    
    return val;
}
