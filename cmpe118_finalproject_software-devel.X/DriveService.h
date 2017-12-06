/* 
 * File:   DriveService.h
 * Author: jwloughb
 *
 * Created on Dec 1, 2017, 3:51 PM
 */

#ifndef DRIVESERVICE_H
#define DRIVESERVICE_H

#include "ES_Configure.h"
#include "IO_Ports.h"
#include <stdint.h>
#include "pwm.h"
#include "Trajectory.h"

#define POST_DRIVE_EVENT PostHsmTopLevel

#define PLUS_INFINITY 2147483647

#define DRIVE_ENABLE_B PWM_PORTY10  // PWM
#define DRIVE_ENABLE_A PWM_PORTY12  // PWM
#define DRIVE_DIRECTION_B PIN5      // Digital Out
#define DRIVE_DIRECTION_A PIN3      // Digital Out
#define DRIVE_DIRECTION_PORT PORTY
#define DRIVE_PWM_FREQ PWM_10KHZ
#define LEFT_BIAS 1000              // Out of 1000
#define RIGHT_BIAS 850              // Out of 1000

// TRACKWIDTH = distance between wheels [milli-inches]
#define TRACKWIDTH 8900
// HALF_TRACKWIDTH = trackwidth/2 [milli-inches]
#define HALF_TRACKWIDTH 4450
// MIN_TURN_RADIUS = tightest turn we can make. More gradual turns = faster robot.
#define MIN_TURN_RADIUS 4450
// Kv = speed-voltage conversion constant [milli-inches/s per volt]
//#define Kv 1550
//#define Ka 1000 // on average good, but slightly inconsistent?
//#define Ke 7595
//#define Kv 1540
//#define Ka 1000 // perfect 90, pretty good 180
//#define Ke 7610
#define Kv 1950
#define Ka 9000 
#define Kw 1600 // pretty good 90, pretty good 180
#define Ke 8000
//#define Kw 1545 // pretty good 90, pretty good 180
//#define Ke 7600
// MAX_VOLTAGE = highest voltage [mV] we want to apply to the motors.
#define MAX_VOLTAGE 9000
// MAX_WHEEL_SPEEED units = [milli-inches/s]. Comes out to ~19 in/s @ 7V.
#define MAX_WHEEL_SPEED (Kv * (MAX_VOLTAGE/1000))
// MAX_FORWARD_SPEED [milli-inches/s] = approximately 9.5 in/sec right now.
#define MAX_FORWARD_SPEED ((MAX_WHEEL_SPEED * MIN_TURN_RADIUS) / (MIN_TURN_RADIUS + HALF_TRACKWIDTH))
// MAX_TURNING_SPEED [milli-radians/s] = approximately 246 deg/s right now.
#define MAX_TURNING_SPEED ((1000 * MAX_WHEEL_SPEED) / HALF_TRACKWIDTH)

/* Periodic Drive Service Functions */
uint8_t InitDriveService(uint8_t priority);
uint8_t PostDriveService(ES_Event thisEvent);
ES_Event RunDriveService(ES_Event thisEvent);

/* Public Interface Functions */
uint8_t EnableDriveMotors(void);
uint8_t DisableDriveMotors(void);
void StopDrive(void);
uint8_t SetForwardSpeed(int32_t mInchesPerSecond);
uint8_t SetTurningSpeed(int32_t degreesPerSecond);
uint8_t SetTurnRadius(int32_t mInches);
void setMotors(int32_t forwardSpeed, int32_t turningSpeed);
void setRightMotor(int32_t voltage);
void setLeftMotor(int32_t voltage);
void InitDistanceTimer(int32_t mInches);
void InitHeadingTimer(int32_t degrees);
uint8_t InitTrajectory(Trajectory t);

#endif  /* DRIVESUBSM_H */
