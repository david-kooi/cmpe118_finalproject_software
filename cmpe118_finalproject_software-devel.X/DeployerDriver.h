#ifndef DEPLOYER_H
#define DEPLOYER_H

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "RC_Servo_mod.h"

#include <stdint.h>

#define ACTION_PERIOD 1000 // ms
#define LOAD_PWM 1000
#define DISPENSE_PWM 1700


uint8_t InitDeployerDriver(uint8_t priority);
uint8_t PostDeployerDriver(ES_Event thisEvent);
ES_Event RunBallDispenser(ES_Event timerEvent);

void DispenseBall(void);











#endif // DEPLOYER_H