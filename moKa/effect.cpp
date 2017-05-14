#include "effect.h"
#include <assert.h>
#include <iostream>

Effect::Effect(unsigned int _filterRBSize,
               unsigned int _delayRBSize,
               bool _delayEnabled) {
    assert(_filterRBSize > 0);

    filterRBSize = _filterRBSize;
    filterRBIndex = 0;
    filterRB = new float[filterRBSize];
    for (unsigned int RBind = 0; RBind < filterRBSize; RBind++)
        filterRB[RBind] = 0.0;

    delayRBSize = _delayRBSize;
    delayRBIndex = 0;
    delayRB = new float[delayRBSize];
    for (unsigned int RBind = 0; RBind < delayRBSize; RBind++)
        delayRB[RBind] = 0.0;

    delayEnabled = _delayEnabled;
    delayStep = 4096*2;
    delayFac = 0.25;
    numDelaySteps = 10;
}

Effect::~Effect() {
    delete [] filterRB;
    delete [] delayRB;
    filterRB = 0;
    delayRB = 0;
}

Effect::Effect(const Effect &effect) {
    filterRBSize = effect.filterRBSize;
    filterRBIndex = effect.filterRBSize;
    filterRB = new float[filterRBSize];

    std::copy(effect.filterRB, effect.filterRB + filterRBSize, filterRB);

    delayRBSize = effect.delayRBSize;
    delayRBIndex = effect.delayRBIndex;
    delayRB = new float[delayRBSize];

    std::copy(effect.delayRB, effect.delayRB + delayRBSize, delayRB);

    delayStep = effect.delayStep;
    delayFac = effect.delayFac;
    numDelaySteps = effect.numDelaySteps;
    delayEnabled = effect.delayEnabled;
}

void
Effect::filterRBAddData(float *data,
                        unsigned int size) {
    for (unsigned int indSample = 0; indSample < size; indSample++) {
        filterRBIndex = (filterRBIndex + 1) % filterRBSize;
        filterRB[filterRBIndex] = data[indSample];
    }
}

void
Effect::filterConvolve(Filter *filter,
                       float *outputData,
                       unsigned int datasize) {
    assert(datasize < filterRBSize);

    unsigned int RBindex = 0;
    if (datasize > filterRBIndex + 1) {
        RBindex = filterRBSize;
    }
    RBindex += filterRBIndex - datasize + 1;

    bool zerodata = true;
    for (unsigned int dataInd = 0; dataInd < datasize; dataInd++) {
        outputData[dataInd] = 0.0;
        unsigned int inputIndex = (RBindex + dataInd) % filterRBSize;
        if (filterRB[inputIndex] != 0.0) zerodata = false;
    }

    if (zerodata) return;

    for (unsigned int dataInd = 0; dataInd < datasize; dataInd++) {

        unsigned int inputIndex = (RBindex + dataInd) % filterRBSize;
      //  outputData[dataInd] = filterRB[inputIndex];

        for (unsigned int IRindex = 0 ; IRindex < filter->size; IRindex++) {
            if (inputIndex == 0) {
                inputIndex = filterRBSize - 1;
            } else {
                inputIndex--;
            }

            //std::cout << IRindex << " " << filter->IR[IRindex] << std::endl;
            outputData[dataInd] += filter->IR[IRindex] * filterRB[inputIndex];
        }
    }
}

void
Effect::delayRBAddData(float *data,
                        unsigned int size) {
    for (unsigned int indSample = 0; indSample < size; indSample++) {
        delayRBIndex = (delayRBIndex + 1) % delayRBSize;
        delayRB[delayRBIndex] = data[indSample];
    }
}

void
Effect::delayRBapply(float *outputData,
                     unsigned int datasize) {
    assert(datasize < delayRBSize);

    unsigned int RBindex = 0;
    if (datasize > delayRBIndex + 1) {
        RBindex = delayRBSize;
    }
    RBindex += delayRBIndex - datasize + 1;

    /*
    bool zerodata = true;
    for (unsigned int dataInd = 0; dataInd < datasize; dataInd++) {
        outputData[dataInd] = 0.0;
        unsigned int inputIndex = (RBindex + dataInd) % delayRBSize;
        if (delayRB[inputIndex] != 0.0) zerodata = false;
    }

    if (zerodata) return;
    */

    for (unsigned int dataInd = 0; dataInd < datasize; dataInd++) {

        unsigned int inputIndex = (RBindex + dataInd) % delayRBSize;

        for (unsigned int indDelay = 0 ; indDelay < numDelaySteps; indDelay++) {
            if (indDelay * delayStep < delayRBSize) {

                outputData[dataInd] += delayRB[inputIndex] * exp(-delayFac * (float)indDelay);
                if (delayStep > inputIndex) {
                    inputIndex += delayRBSize;
                }
                inputIndex -= delayStep;
            }
        }
    }
}
