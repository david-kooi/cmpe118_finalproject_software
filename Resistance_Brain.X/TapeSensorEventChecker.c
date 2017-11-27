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
#define TS_SAMPLE_TIME 4 // 2ms

// Tape Sensor Port Numbers
#define TS_LEFT_TRIG_NUM      PORTY, PIN9
#define TS_LEFT_ADC           AD_PORTV8
#define TS_LEFT_LED_NUM       PORTY, PIN10


// Sensor Thresholds
#define TS_LEFT_HI_THRESH 900
#define TS_LEFT_LO_THRESH 700


 // Forward Declared Functions
 uint16_t CheckTapeSensor_LEFT(void);
 
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
    
    /* LEFT Sensor */
    
    // Trigger Pin
    IO_PortsSetPortOutputs(TS_LEFT_TRIG_NUM);
    IO_SOURCE(TS_LEFT_TRIG_NUM); // Start emitter off
    // ADC Pin
    AD_AddPins(TS_LEFT_ADC);
    // LED Indicator
    IO_PortsSetPortOutputs(TS_LEFT_LED_NUM);
    IO_SOURCE(TS_LEFT_LED_NUM); // Set indicator off
    
  
    
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
    
    
    

    // Continue if we are in initial sampling state
    switch(TS_EMITTER_STATE){
        
        case TS_EMITTER_ON:
            TAPE_PRINT("TS DRIVER STATE: EMITTER ON");

            // Sample all sensors with their emitters on
            
           /* LEFT SENSOR */
            
                IO_SINK(TS_LEFT_TRIG_NUM); // Turn emitter on
                leftSensorVal = CheckTapeSensor_LEFT();
                IO_SOURCE(TS_LEFT_TRIG_NUM); // Turn emitter off

            /* RIGHT SENSOR */
            
            
            // Change State and Restart Sensor
            TS_EMITTER_STATE = TS_EMITTER_OFF;
            ES_Timer_InitTimer(TS_SYNC_TIMER, TS_SAMPLE_TIME); // 2ms Tape Sensor Sampling            
            break;
        
        case TS_EMITTER_OFF:
            TAPE_PRINT("TS DRIVER STATE: EMITTER OFF");

                // Sample all sensors with their emitters off
                // Emitters are turned off after TS_EMITTER_ON sampling
                
                /* LEFT SENSOR */
                    tmp_1 = CheckTapeSensor_LEFT();
                    //leftSensorVal = abs((int32_t)tmp_1 - leftSensorVal);
                    //TAPE_PRINT("DELTA: %d", leftSensorVal);
                    if     (leftSensorVal > TS_LEFT_HI_THRESH){sensorState_CURR |= TS_LEFT_SENSOR;}
                    else if(leftSensorVal < TS_LEFT_LO_THRESH){sensorState_CURR &= ~TS_LEFT_SENSOR;}
                    
                    // Check for change
                    currState = sensorState_PREV & TS_LEFT_SENSOR; // previous state
                    prevState = sensorState_CURR & TS_LEFT_SENSOR; // current state
                    
                    if( prevState ^ currState ){  // There has been a change
                        // Post to Services TODO
                        
                        // Toggle LED
                        currState == 1 ? IO_SINK(TS_LEFT_LED_NUM) : IO_SOURCE(TS_LEFT_LED_NUM);
                    }
                    
                    sensorState_PREV = sensorState_CURR;
                    
                    
                 
                    
                // Sampling finished
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
 * @Function CheckTapeSensor_LEFT(void)
 * @param none
 * @return TRUE or FALSE
 * @brief ...
 * @note 
 * @author David Kooi, 2017.11.21
 * @modified */
uint16_t CheckTapeSensor_LEFT(void){
    static uint16_t val = 0;
    val = AD_ReadADPin(TS_LEFT_ADC);
    TAPE_PRINT("TAPE SENSOR LEFT: %d\r\n", val);
    
    return val;
}