#include "EventCheckerCommon.h"


#define RR_BUMPER_MASK 0x08
#define RL_BUMPER_MASK 0x04
#define FR_BUMPER_MASK 0x02
#define FL_BUMPER_MASK 0x01


#define RR_BUMPER_PIN PIN6
#define RL_BUMPER_PIN PIN7
#define FR_BUMPER_PIN PIN4
#define FL_BUMPER_PIN PIN9

#define RR_BUMPER_NUM PORTY, RR_BUMPER_PIN
#define RL_BUMPER_NUM PORTY, RL_BUMPER_PIN
#define FR_BUMPER_NUM PORTY, FR_BUMPER_PIN
#define FL_BUMPER_NUM PORTY, FL_BUMPER_PIN


/**
 * @Function ReadBumpers(void)
 * @param none
 * @return uint8_t with bumper states encoded as bits
 * @brief This function checks all bumpers in sequence. Events are generated 
 *        for each bumper pressed. TRUE is returned if any bumper event was
 *        generated.
 * @note 
 * @author David Kooi, Ben Swandon, 2017.10.3 14:08
 * @modified */
uint8_t ReadBumpers();

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