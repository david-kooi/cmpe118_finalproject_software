/* 
 * File:   DriveService.c
 * Author: jwloughb
 *
 * Created on Dec 1, 2017, 3:51 PM
 */

#include "BOARD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "DriveService.h"
#include "BatteryEventChecker.h"
#include "pwm.h"
#include "AD.h"
#include <stdint.h>
#include <stdio.h>

#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#define SIGN(x) (((x) > 0) ? 1 : (-1))

#define LEFT_ENABLE_PIN DRIVE_ENABLE_A
#define RIGHT_ENABLE_PIN DRIVE_ENABLE_B
#define LEFT_DIRECTION_PIN DRIVE_DIRECTION_A
#define RIGHT_DIRECTION_PIN DRIVE_DIRECTION_B

static uint8_t newCommand;
static int32_t forwardSpeed;            // [milli-inches/s]
static int32_t turningSpeed;            // [degrees/s]
static uint16_t batteryVoltage;         // [millivolts]

static uint8_t distanceTimerActive;
static uint8_t headingTimerActive;
static int32_t timerDistance;           // [micro-inches]
static int32_t timerHeading;            // [micro-radians]
static int32_t timeoutDistance;         // [micro-inches]
static int32_t timeoutHeading;          // [micro-radians]

typedef enum {
    DISABLED,
    ENABLED,
    TRAJECTORY
} DriveServiceState;
static DriveServiceState controlState;

static uint8_t myPriority;

uint8_t InitDriveService(uint8_t priority) {
    myPriority = priority;
    AD_AddPins(BAT_VOLTAGE);
    batteryVoltage = AD_ReadADPin(BAT_VOLTAGE);
    IO_PortsSetPortOutputs(DRIVE_DIRECTION_PORT,
                           LEFT_DIRECTION_PIN | RIGHT_DIRECTION_PIN);
    IO_PortsClearPortBits(DRIVE_DIRECTION_PORT,
                          LEFT_DIRECTION_PIN | RIGHT_DIRECTION_PIN);
    PWM_SetFrequency(DRIVE_PWM_FREQ);
    PWM_AddPins(LEFT_ENABLE_PIN | RIGHT_ENABLE_PIN);
    PWM_SetDutyCycle(LEFT_ENABLE_PIN, 0);
    PWM_SetDutyCycle(RIGHT_ENABLE_PIN, 0);
    forwardSpeed = 0;
    turningSpeed = 0;
    newCommand = FALSE;
    controlState = DISABLED;
    ES_Timer_InitTimer(DRIVE_SERVICE_TIMER, DRIVE_TIMER_TICKS);
    ES_Event thisEvent;
    thisEvent.EventType = ES_INIT;
    return ES_PostToService(myPriority, thisEvent);
}

uint8_t PostDriveService(ES_Event thisEvent) {
    return ES_PostToService(myPriority, thisEvent);
}

ES_Event RunDriveService(ES_Event thisEvent){
    ES_Event returnEvent;
    returnEvent.EventType = ES_NO_EVENT;
    switch (thisEvent.EventType) {
        case ES_TIMEOUT:
            if (thisEvent.EventParam == DRIVE_SERVICE_TIMER){
                ES_Timer_InitTimer(DRIVE_SERVICE_TIMER, DRIVE_TIMER_TICKS);
                SetForwardSpeed(MAX_FORWARD_SPEED);
                SetTurningSpeed(0);
                int32_t actualForwardSpeed;
                int32_t actualTurningSpeed;
                switch (controlState) {
                    case DISABLED:
                        PWM_SetDutyCycle(LEFT_ENABLE_PIN, 0);
                        PWM_SetDutyCycle(RIGHT_ENABLE_PIN, 0);
                        actualForwardSpeed = 0;
                        actualTurningSpeed = 0;
                    break;
                    case ENABLED:
                        if (newCommand) {
                            newCommand = FALSE;
                            setMotors(forwardSpeed, turningSpeed);
                        }
                        actualForwardSpeed = forwardSpeed;
                        actualTurningSpeed = turningSpeed;
                    break;
                    case TRAJECTORY:
                        // Do trajectory magic
                        actualForwardSpeed = 0; // fix these too
                        actualTurningSpeed = 0;
                    break;
                    default:
                        printf("Illegal drive control state encountered.\r\n");
                    break;
                }
                if (distanceTimerActive) {
                    timerDistance += 50 * actualForwardSpeed;
                    if (timerDistance >= timeoutDistance) {
                        returnEvent.EventType = DISTANCE_TIMEOUT;
                        POST_DRIVE_EVENT(returnEvent);
                        distanceTimerActive = FALSE;
                    }
                }
                if (headingTimerActive) {
                    timerHeading += 50 * actualTurningSpeed;
                    if (timerHeading >= timeoutHeading) {
                        returnEvent.EventType = HEADING_TIMEOUT;
                        POST_DRIVE_EVENT(returnEvent);
                        headingTimerActive = FALSE;
                    }
                }
            }
            break;
        default:
            break;
    }
    return returnEvent;
}

/************************* Public Interface Functions *************************/

uint8_t EnableDriveMotors(void) {
    if (controlState != DISABLED) {
        return ERROR;
    }
    controlState = ENABLED;
    return SUCCESS;
}

uint8_t DisableDriveMotors(void) {
    if (controlState == DISABLED) {
        return ERROR;
    }
    controlState = DISABLED;
    return SUCCESS;
}

void StopDrive(void) {
    PWM_SetDutyCycle(LEFT_ENABLE_PIN, 0);
    PWM_SetDutyCycle(RIGHT_ENABLE_PIN, 0);
    forwardSpeed = 0;
    turningSpeed = 0;
    newCommand = TRUE;
    return;
}

uint8_t SetForwardSpeed(int32_t mInchesPerSecond) {
    if (controlState != ENABLED) {
        return ERROR;
    }
    if (ABS(mInchesPerSecond) > MAX_FORWARD_SPEED) {
        forwardSpeed = SIGN(mInchesPerSecond) * MAX_FORWARD_SPEED;
    } else {
        forwardSpeed = mInchesPerSecond;
    }
    newCommand = TRUE;
    return SUCCESS;
}

uint8_t SetTurningSpeed(int32_t degreesPerSecond) {
    if (controlState != ENABLED) {
        return ERROR;
    }
    int32_t mRadiansPerSecond = (31416 * degreesPerSecond) / 1800;
    if (ABS(mRadiansPerSecond) > MAX_TURNING_SPEED) {
        turningSpeed = SIGN(mRadiansPerSecond) * MAX_TURNING_SPEED;
    } else {
        turningSpeed = mRadiansPerSecond;
    }
    newCommand = TRUE;
    return SUCCESS;
}

uint8_t SetTurnRadius(int32_t mInches) {
    if (controlState != ENABLED) {
        return ERROR;
    }
    if (mInches == (int32_t)PLUS_INFINITY || ABS(mInches) < MIN_TURN_RADIUS) {
        turningSpeed = 0;
    } else {
        turningSpeed = (1000 * forwardSpeed) / mInches;
    }
    newCommand = TRUE;
    return SUCCESS;
}

void InitDistanceTimer(int32_t mInches) {
    timeoutDistance = 1000 * mInches; // convert from milli-inches to micro-inches
    timerDistance = 0;
    distanceTimerActive = TRUE;
    return;
}

void InitHeadingTimer(int32_t degrees) {
    timeoutHeading = 1000 * (31416 * degrees) / 1800; // convert from milli-radians to micro-radians
    timerHeading = 0;
    headingTimerActive = TRUE;
    return;
}

void setMotors(int32_t forwardSpeed, int32_t turningSpeed) {
    batteryVoltage = GetBatteryVoltage();
    int32_t leftSpeed = forwardSpeed - ((turningSpeed * HALF_TRACKWIDTH) / 1000);
    int32_t rightSpeed = forwardSpeed + ((turningSpeed * HALF_TRACKWIDTH) / 1000);
    int32_t leftVoltage = (LEFT_BIAS * leftSpeed) / Kv;
    int32_t rightVoltage = (RIGHT_BIAS * rightSpeed) / Kv;
    setLeftMotor(leftVoltage);
    setRightMotor(rightVoltage);
    return;
}
void setLeftMotor(int32_t voltage) {
    uint32_t dutyCycle = (1000 * ABS(voltage)) / GetBatteryVoltage();
    if (voltage < 0) {
        IO_PortsSetPortBits(DRIVE_DIRECTION_PORT, LEFT_DIRECTION_PIN);
    }else{
        IO_PortsClearPortBits(DRIVE_DIRECTION_PORT, LEFT_DIRECTION_PIN);
    }
    printf("LEFT DUTY %d\r\n", dutyCycle);
    PWM_SetDutyCycle(LEFT_ENABLE_PIN, dutyCycle);
    return;
}

void setRightMotor(int32_t voltage) {
    
    uint32_t dutyCycle = (1000 * ABS(voltage)) / GetBatteryVoltage();
    if (voltage < 0) {
        IO_PortsSetPortBits(DRIVE_DIRECTION_PORT, RIGHT_DIRECTION_PIN);
    }else{
        IO_PortsClearPortBits(DRIVE_DIRECTION_PORT, RIGHT_DIRECTION_PIN);
    }
    
    printf("RIGHT DUTY %d\r\n",dutyCycle);
    PWM_SetDutyCycle(RIGHT_ENABLE_PIN, dutyCycle);
    
    return;
}
