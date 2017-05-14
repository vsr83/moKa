#ifndef EFFECT_H
#define EFFECT_H

#include "filter.h"

class Effect {
public:
    Effect(unsigned int _filterRBSize = 44100,
           unsigned int _delayRBSize = 44100 * 10,
           bool _delayEnabled = true);
    ~Effect();
    Effect(const Effect &effect);

    void filterRBAddData(float *data,
                         unsigned int size);
    void filterConvolve(Filter *filter,
                        float *outputData,
                        unsigned int datasize);
    void delayRBAddData(float *data,
                        unsigned int size);
    void delayRBapply(float *outputData,
                      unsigned int datasize);

    bool delayEnabled;
private:
    float *filterRB;
    unsigned int filterRBSize;
    unsigned int filterRBIndex;

    float *delayRB;
    unsigned int delayRBSize;
    unsigned int delayRBIndex;

    unsigned int delayStep;
    float delayFac;
    unsigned int numDelaySteps;

};

#endif // EFFECT_H
