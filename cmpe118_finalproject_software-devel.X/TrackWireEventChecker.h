/* 
 * File:   TrackWireEventChecker.h
 * Author: dkooi
 *
 * Created on November 27, 2017, 7:42 PM
 */

#ifndef TRACKWIREEVENTCHECKER_H
#define	TRACKWIREEVENTCHECKER_H

#include "EventCheckerCommon.h"

#define TW_LEFT_ADC  AD_PORTW4
#define TW_RIGHT_ADC AD_PORTW6

#define TRACKWIRE_IN_SIGHT_THRESHOLD 75
#define TRACKWIRE_TOUCHING_THRESHOLD 500
#define TRACKWIRE_WEAK_IN_SIGHT_THRESHOLD 50

#define LEFT_TRACKWIRE_DETECTED 1
#define LEFT_TRACKWIRE_TOUCHING_DETECTED 2

#define RIGHT_TRACKWIRE_DETECTED 3
#define RIGHT_TRACKWIRE_TOUCHING_DETECTED 4

#define TRACKWIRE_NOT_DETECTED 5

void InitializeTrackWire(void);

uint8_t RightTrackWireCheck(void);

uint8_t LeftTrackWireCheck(void);

uint16_t TW_GetRightReading();
uint16_t TW_GetLeftReading();

#endif	/* TRACKWIREEVENTCHECKER_H */

