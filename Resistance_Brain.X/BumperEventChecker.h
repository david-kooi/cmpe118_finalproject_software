#include "EventCheckerCommon.h"



/**
 * @Function CheckBumpers(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function checks all bumpers in sequence. Events are generated 
 *        for each bumper pressed.
 * @note 
 * @author David Kooi, Ben Swandon, 2017.10.3 14:08
 * @modified */
uint8_t CheckBumpers(void);

/**
 * @Function InitializeBumperChecker(void)
 * @param none
 * @return none
 * @brief Initializes variables related to the CheckBumper function.
 * @author David Kooi, 2017.10.03 14:30
 * @modified 
 */
void InitializeBumperChecker(void);