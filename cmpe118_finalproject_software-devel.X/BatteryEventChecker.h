/* 
 * File:   BatteryEventChecker.h
 * Author: jwloughb
 *
 * Created on Dec 2, 2017, 4:55 PM
 */

#ifndef BATTERYEVENTCHECKER_H
#define BATTERYEVENTCHECKER_H

#include <stdint.h>

#define POST_BATTERY_EVENT PostHsmTopLevel

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
uint8_t CheckBatteryEvent(void);

// GetBatteryVoltage(): Returns last measured battery voltage in millivolts
int32_t GetBatteryVoltage(void);

#endif
