#include <BOARD.h>
#include <xc.h>
#include <stdio.h>
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_Timers.h"
#include "AD.h"

#include "BumperEventChecker.h"
#include "TapeSensorEventChecker.h"
#include "TrackWireEventChecker.h"
#include "Stepper.h"

//Test
void TestADCPins(void);
void main(void)
{
    ES_Return_t ErrorType;

    BOARD_Init();
    
    DEBUG_PRINT("STARTING");
     
    printf("Starting ES Framework Template\r\n");
    printf("using the 2nd Generation Events & Services Framework\r\n");

    // Your hardware initialization function calls go here
    AD_Init();
    AD_AddPins(TW_LEFT_ADC | TW_RIGHT_ADC | TS_LEFT_ADC | TS_RIGHT_ADC | TS_CENTER_ADC | TS_REAR_ADC);
    PWM_Init();
    
    //TestADCPins(); // Cycles through and prints values from pins forever
    //Stepper_Test(10, 10);
  
    /* Initialize Event Checkers */
    
    InitializeBumperChecker();
    InitializeTrackWire();
    InitializeTapeSensors(); 

    // now initialize the Events and Services Framework and start it running
    ErrorType = ES_Initialize();
    if (ErrorType == Success) {
        ErrorType = ES_Run();

    }
    //if we got to here, there was an error
    switch (ErrorType) {
    case FailedPointer:
        printf("Failed on NULL pointer");
        break;
    case FailedInit:
        printf("Failed Initialization");
        break;
    default:
        printf("Other Failure: %d", ErrorType);
        break;
    }
    for (;;)
        ;

};



void TestADCPins(void){
    uint16_t val;
    AD_AddPins(0xFFF);
    
    while(1){
        int i;
        for(i = 0; i < 1000000; i++){
        
        }
        
        val = AD_ReadADPin(AD_PORTV3);
        printf("V3: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTV4);
        printf("V4: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTV5);
        printf("V5: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTV6);
        printf("V6: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTV7);
        printf("V7: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTV8);
        printf("V8: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTW3);
        printf("W3: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTW4);
        printf("W4: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTW5);
        printf("W5: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTW6);
        printf("W6: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTW7);
        printf("W7: %d\r\n", val);

        val = AD_ReadADPin(AD_PORTW8);
        printf("W8: %d\r\n", val);
   
    }
    
}


/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
