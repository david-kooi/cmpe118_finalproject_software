/* 
 * File:   Trajectory.h
 * Author: jwloughb
 *
 * Created on Dec 4, 2017, 4:53 PM
 */

#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <stdint.h>
#include <string.h>

typedef struct motion {
    int16_t v;
    int16_t vDot;
    int16_t w;
    int16_t wDot;
} MotionState;

typedef struct traj {
    uint16_t length;
    MotionState motionState[30];
} Trajectory;


__attribute__((space(prog))) Trajectory pivot135Degrees = {
	28,
	{{0, 0, 0, 0},
	{0, 0, 54, 1972},
	{0, 0, 207, 3944},
	{0, 0, 458, 5916},
	{0, 0, 808, 7888},
	{0, 0, 1257, 9860},
	{0, 0, 1775, 9860},
	{0, 0, 2213, 7888},
	{0, 0, 2554, 5916},
	{0, 0, 2795, 3944},
	{0, 0, 2938, 1972},
	{0, 0, 2983, 0},
	{0, 0, 2983, 0},
	{0, 0, 2983, 0},
	{0, 0, 2983, 0},
	{0, 0, 2983, 0},
	{0, 0, 2980, -394},
	{0, 0, 2906, -2366},
	{0, 0, 2733, -4338},
	{0, 0, 2462, -6310},
	{0, 0, 2092, -8282},
	{0, 0, 1624, -10254},
	{0, 0, 1112, -9465},
	{0, 0, 693, -7493},
	{0, 0, 373, -5521},
	{0, 0, 151, -3549},
	{0, 0, 28, -1578},
	{0, 0, 0, 0}}
};

__attribute__((space(prog))) Trajectory pivot90Degrees = {
	23,
	{{0, 0, 0, 0},
	{0, 0, 54, 1977},
	{0, 0, 208, 3954},
	{0, 0, 460, 5930},
	{0, 0, 810, 7907},
	{0, 0, 1260, 9884},
	{0, 0, 1767, 9489},
	{0, 0, 2187, 7512},
	{0, 0, 2509, 5535},
	{0, 0, 2731, 3558},
	{0, 0, 2855, 1581},
	{0, 0, 2881, -198},
	{0, 0, 2817, -2174},
	{0, 0, 2654, -4151},
	{0, 0, 2392, -6128},
	{0, 0, 2031, -8105},
	{0, 0, 1572, -10082},
	{0, 0, 1068, -9291},
	{0, 0, 658, -7314},
	{0, 0, 347, -5337},
	{0, 0, 134, -3361},
	{0, 0, 21, -1384},
	{0, 0, 0, 0}}
};

__attribute__((space(prog))) Trajectory pivot45Degrees = {
	19,
	{{0, 0, 0, 0},
	{0, 0, 54, 1953},
	{0, 0, 205, 3906},
	{0, 0, 454, 5859},
	{0, 0, 801, 7812},
	{0, 0, 1190, 7031},
	{0, 0, 1488, 5078},
	{0, 0, 1688, 3125},
	{0, 0, 1791, 1172},
	{0, 0, 1800, -586},
	{0, 0, 1717, -2539},
	{0, 0, 1536, -4492},
	{0, 0, 1258, -6445},
	{0, 0, 882, -8398},
	{0, 0, 516, -6445},
	{0, 0, 247, -4492},
	{0, 0, 76, -2539},
	{0, 0, 3, -586},
	{0, 0, 0, 0}}
};

__attribute__((space(prog)))Trajectory pivot5Degrees = {
	10,
	{{0, 0, 0, 0},
//	{0, 0, 52, 1885},
//	{0, 0, 198, 3769},
//	{0, 0, 353, 2262},
//	{0, 0, 415, 377},
	{0, 0, 3000, 1885},
	{0, 0, 3000, 3769},
	{0, 0, 3000, 2262},
	{0, 0, 3000, 377},
	{0, 0, 382, -1508},
	{0, 0, 254, -3392},
	{0, 0, 86, -2638},
	{0, 0, 6, -754},
	{0, 0, 0, 0}}
};



__attribute__((space(prog)))Trajectory step10Inches = {
	29,
	{{0, 0, 0, 0},
	{205, 7470, 0, 0},
	{784, 14941, 0, 0},
	{1737, 22411, 0, 0},
	{3063, 29882, 0, 0},
	{4557, 29882, 0, 0},
	{6051, 29882, 0, 0},
	{7545, 29882, 0, 0},
	{9039, 29882, 0, 0},
	{10533, 29882, 0, 0},
	{12005, 27641, 0, 0},
	{13182, 20170, 0, 0},
	{13985, 12700, 0, 0},
	{14414, 5229, 0, 0},
	{14482, -1494, 0, 0},
	{14201, -8965, 0, 0},
	{13548, -16435, 0, 0},
	{12521, -23906, 0, 0},
	{11131, -29882, 0, 0},
	{9637, -29882, 0, 0},
	{8143, -29882, 0, 0},
	{6649, -29882, 0, 0},
	{5155, -29882, 0, 0},
	{3661, -29882, 0, 0},
	{2222, -26147, 0, 0},
	{1121, -18676, 0, 0},
	{392, -11206, 0, 0},
	{37, -3735, 0, 0},
	{0, 0, 0, 0}}
};

__attribute__((space(prog)))Trajectory step8Inches = {
	26,
	{{0, 0, 0, 0},
	{547, 19900, 0, 0},
	{1940, 29851, 0, 0},
	{3433, 29851, 0, 0},
	{4925, 29851, 0, 0},
	{6418, 29851, 0, 0},
	{7910, 29851, 0, 0},
	{9403, 29851, 0, 0},
	{10896, 29851, 0, 0},
	{12388, 29851, 0, 0},
	{13881, 29851, 0, 0},
	{14826, 9950, 0, 0},
	{14925, 0, 0, 0},
	{14925, 0, 0, 0},
	{14716, -11940, 0, 0},
	{13582, -29851, 0, 0},
	{12090, -29851, 0, 0},
	{10597, -29851, 0, 0},
	{9104, -29851, 0, 0},
	{7612, -29851, 0, 0},
	{6119, -29851, 0, 0},
	{4627, -29851, 0, 0},
	{3134, -29851, 0, 0},
	{1642, -29851, 0, 0},
	{358, -17910, 0, 0},
	{0, 0, 0, 0}}
};

__attribute__((space(prog)))Trajectory step5Inches = {
	22,
	{{0, 0, 0, 0},
	{205, 7440, 0, 0},
	{781, 14881, 0, 0},
	{1730, 22321, 0, 0},
	{3051, 29762, 0, 0},
	{4539, 29762, 0, 0},
	{6027, 29762, 0, 0},
	{7437, 25298, 0, 0},
	{8497, 17857, 0, 0},
	{9185, 10417, 0, 0},
	{9501, 2976, 0, 0},
	{9468, -3720, 0, 0},
	{9077, -11161, 0, 0},
	{8315, -18601, 0, 0},
	{7180, -26042, 0, 0},
	{5729, -29762, 0, 0},
	{4241, -29762, 0, 0},
	{2757, -29018, 0, 0},
	{1510, -21577, 0, 0},
	{636, -14137, 0, 0},
	{134, -6696, 0, 0},
	{0, 0, 0, 0}}
};

__attribute__((space(prog)))Trajectory step2Inches = {
	14,
	{{0, 0, 0, 0},
	{546, 19871, 0, 0},
	{1937, 29806, 0, 0},
	{3428, 29806, 0, 0},
	{4918, 29806, 0, 0},
	{6200, 17884, 0, 0},
	{6557, 0, 0, 0},
	{6110, -17884, 0, 0},
	{4769, -29806, 0, 0},
	{3279, -29806, 0, 0},
	{1788, -29806, 0, 0},
	{447, -19871, 0, 0},
	{0, 0, 0, 0}}
};



#endif
