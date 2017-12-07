// --- Least-Squares Derivative ---
// This module is adapted from FRC Team 1197's TorDerivative Java class.
//  (See: https://goo.gl/vTQgJ5)
// Author: Joe Loughboro
// Date: 9/18/2017
#ifndef LSDERIVATIVE_H
#define LSDERIVATIVE_H
#include <stdint.h>

typedef struct DerivObj* Derivative;

// newDerivative(): Allocates memory for a new Derivative object
//  and returns a handle to the new object.
Derivative newDerivative(int32_t dt);

// freeDerivative(): Frees heap memory associated with the
//  pointed-to Derivative.
void freeDerivative(Derivative *pd);

// resetDerivative(): Resets a Derivative object to its initial
//  (zero/empty) state.
void resetDerivative(Derivative d);
    
// estimateDerivative(): Uses a first order least squares approximation
//  to calculate the slope of the data. Must be called every dt
//  milliseconds with new data for valid results.
int32_t estimateDerivative(Derivative d, int32_t y);

// getDerivative(): Returns the value computed by the previous
//  call to estimateDerivative().
int32_t getDerivative(Derivative d);

#endif
