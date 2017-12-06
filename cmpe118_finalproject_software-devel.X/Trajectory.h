/* 
 * File:   Trajectory.h
 * Author: jwloughb
 *
 * Created on Dec 4, 2017, 4:53 PM
 */

#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <stdint.h>

typedef struct motion {
    int16_t v;
    int16_t vDot;
    int16_t w;
    int16_t wDot;
} MotionState;

typedef struct traj {
    uint16_t length;
    MotionState motionState[];
} Trajectory;

Trajectory DriveForward10 = {
    50,
    {{0, 0, 0, 0},
    {54, 1960, 0, 0},
    {206, 3920, 0, 0},
    {456, 5880, 0, 0},
    {804, 7840, 0, 0},
    {1250, 9800, 0, 0},
    {1793, 11760, 0, 0},
    {2435, 13720, 0, 0},
    {3173, 15288, 0, 0},
    {3884, 13328, 0, 0},
    {4496, 11368, 0, 0},
    {5011, 9408, 0, 0},
    {5427, 7448, 0, 0},
    {5746, 5488, 0, 0},
    {5966, 3528, 0, 0},
    {6089, 1568, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6116, 0, 0, 0},
    {6110, -588, 0, 0},
    {6027, -2548, 0, 0},
    {5846, -4508, 0, 0},
    {5566, -6468, 0, 0},
    {5189, -8428, 0, 0},
    {4714, -10388, 0, 0},
    {4141, -12348, 0, 0},
    {3469, -14308, 0, 0},
    {2720, -14700, 0, 0},
    {2038, -12740, 0, 0},
    {1455, -10780, 0, 0},
    {970, -8820, 0, 0},
    {583, -6860, 0, 0},
    {294, -4900, 0, 0},
    {103, -2940, 0, 0},
    {10, -980, 0, 0},
    {0, 0, 0, 0}}
};


#endif
