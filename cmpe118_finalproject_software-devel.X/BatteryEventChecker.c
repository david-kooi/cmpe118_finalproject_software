/* 
 * File:   BatteryEventChecker.c
 * Author: jwloughb
 *
 * Created on Dec 2, 2017, 4:55 PM
 */

#include "ES_Configure.h"
#include "BatteryEventChecker.h"
#include "ES_Events.h"
#include "serial.h"
#include "AD.h"

#define BATTERY_DISCONNECT_THRESHOLD 175

#define ADC_TICKS_PER_VOLT 31
static int32_t millivolts = 307; // Corresonds to nominal battery voltage of 9.9V

/**
 * @Function CheckBatteryEvent(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is a prototype event checker that checks the battery voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the actual battery voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either BATTERY_CONNECTED or BATTERY_DISCONNECTED if the power switch is turned
 *        on or off with the USB cord plugged into the Uno32. Returns TRUE if there was an 
 *        event, FALSE otherwise.
 * @note Use this code as a template for your other event checkers, and modify as necessary.
 * @author Gabriel H Elkaim, 2013.09.27 09:18
 * @modified Gabriel H Elkaim/Max Dunne, 2016.09.12 20:08 */
uint8_t CheckBatteryEvent(void) {
    static ES_EventTyp_t lastEvent = BATTERY_DISCONNECTED;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint16_t batVoltage = AD_ReadADPin(BAT_VOLTAGE); // read the battery voltage
    millivolts = (1000 * batVoltage) / ADC_TICKS_PER_VOLT;
    if (batVoltage > BATTERY_DISCONNECT_THRESHOLD) { // is battery connected?
        curEvent = BATTERY_CONNECTED;
    } else {
        curEvent = BATTERY_DISCONNECTED;
    }
    if (curEvent != lastEvent) { // check for change from last time
        thisEvent.EventType = curEvent;
        thisEvent.EventParam = batVoltage;
        returnVal = TRUE;
        lastEvent = curEvent; // update history
        POST_BATTERY_EVENT(thisEvent);
    }
    return (returnVal);
}

int32_t GetBatteryVoltage(void) {
    return millivolts;
}
