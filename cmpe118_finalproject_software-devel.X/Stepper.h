/*
 * File:   Stepper.h
 * Author: Elkaim
 *
 * Software module to drive a stepper motor through a normal H-bridge in full-step drive
 * mode. The module uses TIMER3 and is capable of generated 1/2 to 20,000 steps per second.
 * The nominal port used is PORTZ and can be changed by changing the appropriate #defines
 * below.
 *
 * NOTE: Module uses TIMER3 for its interrupts. When modifying this code for your own
 *       use, note that you will need to alter the #defines for modes below, and that
 *       only one of them can be active at any one time.
 * 
 * MODES: This module uses conditional compilation to set which mode the stepper motor
 *        is operating in. The valid modes are:
 *           FULL_STEP_DRIVE
 *           HALF_STEP_DRIVE
 *           WAVE_DRIVE
 *           DRV8811_DRIVE
 *
 * STEPPER_TEST (in the .c file) conditionally compiles the test harness for the code. 
 * Make sure it is commented out for module useage.
 *
 * Created on January 2, 2012, 9:36 PM
 */

#ifndef Stepper_H
#define Stepper_H

#include <BOARD.h>
#include <IO_Ports.h>

/*******************************************************************************
 * STEPPER MODE #DEFINES                                                       *
 ******************************************************************************/

//#define FULL_STEP_DRIVE
//#define HALF_STEP_DRIVE
//#define WAVE_DRIVE
#define DRV8811_DRIVE

#if defined FULL_STEP_DRIVE && ( defined HALF_STEP_DRIVE || defined WAVE_DRIVE || defined DRV8811_DRIVE )
#error "Define only one stepper drive mode at a time"
#endif

#if defined HALF_STEP_DRIVE && ( defined FULL_STEP_DRIVE || defined WAVE_DRIVE || defined DRV8811_DRIVE )
#error "Define only one stepper drive mode at a time"
#endif

#if defined WAVE_DRIVE && ( defined HALF_STEP_DRIVE || defined FULL_STEP_DRIVE || defined DRV8811_DRIVE )
#error "Define only one stepper drive mode at a time"
#endif

#if defined DRV8811_DRIVE && ( defined HALF_STEP_DRIVE || defined FULL_STEP_DRIVE || defined WAVE_DRIVE )
#error "Define only one stepper drive mode at a time"
#endif

/*******************************************************************************
 * PUBLIC #DEFINES                                                             *
 ******************************************************************************/

#define FORWARD 1
#define REVERSE 0

#define TRIS_COIL_A_ENABLE      PORTX05_TRIS
#define TRIS_COIL_A_DIRECTION   PORTX04_TRIS
#define TRIS_COIL_B_ENABLE      PORTX03_TRIS
#define TRIS_COIL_B_DIRECTION   PORTX06_TRIS

#define COIL_A_ENABLE       PORTX03_LAT
#define COIL_A_DIRECTION    PORTX04_LAT
#define COIL_B_ENABLE       PORTX05_LAT
#define COIL_B_DIRECTION    PORTX06_LAT

#define DRV8811_STEP      PORTX03_LAT
#define DRV8811_DIRECTION PORTX04_LAT
#define DRV8811_ENABLE    PORTX05_LAT

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/**
 * @Function: Stepper_Test(void);
 * @param none
 * @return SUCCESS or ERROR
 * @brief Runs a blocking stepper test routine that drives a stepper motor forward and backward.
 * @note Uses the timers.h instead of ES_Timers.h
 * @author Joe Loughboro, 2017.12.3 16:44 */
void Stepper_Test(uint16_t steps, uint16_t rate);

/**
 * @Function: Stepper_Init(void);
 * @param none
 * @return SUCCESS or ERROR
 * @brief Initializes the Stepper Module, sets up the stepper on PORTZ
 * @note Defaults to a stepping rate of MED_HZ_RATE
 * @author Gabriel Hugh Elkaim, 2016.10.13 15:37 */
int8_t Stepper_Init(void);

/**
 * @Function Stepper_SetRate(unsigned short int rate);
 * @param rate, steps per second, 0 is a special case of 0.5Hz
 * @return SUCCESS or ERROR
 * @brief Sets the stepping rate in steps per second
 * @author Gabriel Hugh Elkaim, 2016.10.13 15:37 */
int8_t Stepper_SetRate(uint16_t rate);

/**
 * @Function Stepper_GetRate(void);
 * @param none
 * @return rate in steps per second
 * @brief Gets the stepping rate in steps per second
 * @author Gabriel Hugh Elkaim, 2016.10.13 15:37 */
uint16_t Stepper_GetRate(void);

/**
 * @Function: Stepper_SetSteps(char direction, unsigned int steps);
 * @param direction - stepper direction (FORWARD or REVERSE)
 * @param steps - number of steps to take
 * @return SUCCESS or ERROR
 * @brief Sets the number of steps and direction for the stepper drive. Wipes
 *        out any previous values, does NOT start stepping. If the stepper is
 *        already stepping, then the stepper will continue stepping.
 * @author Gabriel Hugh Elkaim, 2016.10.13 15:37 */
int8_t Stepper_SetSteps(uint8_t direction, int32_t steps);

/**
 * @Function: Stepper_InitSteps(char direction, unsigned int steps);
 * @param direction - stepper direction (FORWARD or REVERSE)
 * @param steps - number of steps to take
 * @return SUCCESS or ERROR
 * @brief Sets the number of steps and direction for the stepper drive. Wipes
 *        out any previous stepper command and starts stepping.
 * @author Gabriel Hugh Elkaim, 2016.10.13 15:37 */
int8_t Stepper_InitSteps(uint8_t direction, int32_t steps);

/**
 * @Function: Stepper_StartSteps(void);
 * @param none
 * @return SUCCESS or ERROR
 * @brief Starts the stepper motor stepping
 * @author Gabriel Hugh Elkaim, 2016.10.13 15:37 */
int8_t Stepper_StartSteps(void);

/**
 * @Function: Stepper_StopSteps(void);
 * @param none
 * @return SUCCESS or ERROR
 * @brief Stops the stepper motor stepping
 * @author Gabriel Hugh Elkaim, 2016.10.13 15:37 */
int8_t Stepper_StopsSteps(void);

/**
 * @Function: Stepper_GetRemainingSteps(void);
 * @return number of remaining steps
 * @brief Returns the number of remaining steps
 * @author Gabriel Hugh Elkaim, 2012.01.28 23:21 */
int32_t Stepper_GetRemainingSteps(void);

/**
 * @Function: Stepper_GetDirection(void);
 * @return FORWARD or REVERSE
 * @brief Returns the current direction (or previous if stopped)
 * @author Gabriel Hugh Elkaim, 2012.01.28 23:21 */
int8_t Stepper_GetDirection(void);

/**
 * @Function: Stepper_IsStepping(void);
 * @return TRUE or FALSE
 * @brief Returns TRUE if the stepper is currently stepping, FALSE otherwise
 * @author Gabriel Hugh Elkaim, 2016.10.13 15:37 */
int8_t Stepper_IsStepping(void);

/**
 * @Function: Stepper_End(void);
 * @return SUCCESS or ERROR
 * @brief Shuts down the stepper driver software module. Module can be re-enabled
 *        using a stepper init.
 * @author Gabriel Hugh Elkaim, 2012.01.28 23:21 */
int8_t Stepper_End(void);



#endif
