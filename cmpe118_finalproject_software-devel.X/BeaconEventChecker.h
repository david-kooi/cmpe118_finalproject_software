#ifndef BEACON_EVENTCHECKER_H
#define	BEACON_EVENTCHECKER_H

#include "EventCheckerCommon.h"
#define BEACON_ADC  AD_PORTV4


#define LO_IN_SIGHT_THRESH 675
#define HI_IN_SIGHT_THRESH 750

#define LO_HEAD_ON_THRESH  HI_IN_SIGHT_THRESH
#define HI_HEAD_ON_THRESH  800

void CheckBeacon(void);






#endif	/* BEACON_EVENTCHECKER_H */

