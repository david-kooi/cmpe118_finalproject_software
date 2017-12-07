// --- Least-Squares Derivative ---
// This module is adapted from FRC Team 1197's TorDerivative Java class.
//  (See: https://goo.gl/vTQgJ5)
// Author: Joe Loughboro
// Date: 9/18/2017
#include "LsDerivative.h"
#include <stdint.h>
#include <stdlib.h>

#define LENGTH 64
#define LENGTH_MINUS_1 63
#define LOG2_LENGTH 6
#define MOD_LENGTH(x) ((x) & LENGTH_MINUS_1)

static int32_t SStx(Derivative d);

typedef struct DerivObj {
    int32_t dt;
    int32_t *yData;
    int32_t frontIndex;
    int32_t SStt;
    int32_t tMean;
    int32_t preMean;
    int32_t v;
} derivObj;

Derivative newDerivative(int32_t dt){
    Derivative d = (Derivative) calloc(1, sizeof(derivObj));
    if (d != (Derivative) 0) {
        d->dt = dt;
        d->yData = (int32_t) calloc(LENGTH, sizeof(int32_t));
        d->tMean = ((LENGTH - 1) * d->dt) >> 1;
        // Note the symmetry between this and the SStx() method:
        d->SStt = 0;
        
        uint32_t i;
        for(i = 0;  i < LENGTH; ++i){
            d->SStt += ((i * d->dt) - d->tMean)
                     * ((i * d->dt) - d->tMean);
        }
        return d;
    }
    return (Derivative) 0;
}

void freeDerivative(Derivative *pd) {
    Derivative d = *pd;
    if (d->yData != (int32_t *) 0) {
        free(d->yData);
    }
    free(d);
    *pd = (Derivative) 0;
    return;
}

void resetDerivative(Derivative d){
    d->v = 0;
    d->frontIndex = 0;
    d->preMean = 0;
    uint32_t i;
    for (i = 0; i < LENGTH; ++i) {
        d->yData[i] = 0;
    }
    return;
}

int32_t estimateDerivative(Derivative d, int32_t y) {
    d->preMean -= d->yData[MOD_LENGTH(d->frontIndex)];
    d->yData[MOD_LENGTH(d->frontIndex)] = y;
    d->preMean += d->yData[MOD_LENGTH(d->frontIndex)];
    int32_t SSty = SStx(d);
    d->frontIndex = MOD_LENGTH(d->frontIndex + 1);
    d->v = (1000 * SSty) / d->SStt;
    return d->v;
}

int32_t getDerivative(Derivative d) {
    return d->v;
}

static int32_t SStx(Derivative d){
    int32_t mean  = d->preMean >> LOG2_LENGTH;
    int32_t sum = 0;
    uint32_t i;
    for (i = 0; i < LENGTH; ++i) {
        sum += ((i * d->dt) - d->tMean)
             * (d->yData[MOD_LENGTH(i + d->frontIndex)] - mean);
    }
    return sum;
}
