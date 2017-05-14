#ifndef EFFECT_H
#define EFFECT_H

#include "filter.h"

class Effect {
public:
    Effect(unsigned int _filterRBSize = 44100);
    ~Effect();
    Effect(const Effect &effect);

    void filterRBAddData(float *data,
                         unsigned int size);
    void filterConvolve(Filter *filter,
                        float *outputData,
                        unsigned int datasize);

private:
    float *filterRB;
    unsigned int filterRBSize;
    unsigned int filterRBIndex;
};

#endif // EFFECT_H
