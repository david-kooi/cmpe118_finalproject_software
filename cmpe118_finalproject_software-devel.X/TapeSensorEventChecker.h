#ifndef EVENTCHECKER_H
#define	EVENTCHECKER_H

#include "EventCheckerCommon.h"


/**
 * @Function InitializeTapeSensors(void)
 * @param none
 * @return TRUE or FALSE
 * @brief ...
 * @note 
 * @author David Kooi, Ben Swandon, 2017.10.3 14:08
 * @modified */
uint8_t InitializeTapeSensors(void);

/**
 * @Function CheckTapeSensors(void)
 * @param none
 * @return TRUE if an event occured or FALSE if no event occured
 * @brief Start asynchronously sync-sampling the tape sensors 
 * @note Subsequent calls to StartPing() will have no effect until the initial
   ping is complete.
 * @author David Kooi, 2017.11.21
 * @modified */
void StartSampling(void);

/**
 * @Function TS_DriveSampling(void)
 * @param none
 * @return 
 * @brief 
 * @note 
 * @author David Kooi, 2017.11.21
 * @modified */
void TS_DriveSampling(void);




#endif	/* TEMPLATEEVENTCHECKER_H */

