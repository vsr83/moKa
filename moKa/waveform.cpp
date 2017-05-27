/*  MöKä -
 *
 *  Copyright (C) 2017 Ville Räisänen <vsr at vsr.name>
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "waveform.h"
#include <math.h>
#include <assert.h>
#include <algorithm>
#include <iostream>

double
Waveform::evalSin(double tnorm) {
    return sin(tnorm);
}

double
Waveform::evalTri(double tnorm) {
    double tmod = fmod(tnorm, 2.0 * M_PI) - M_PI;
    return 1.0 - 2.0 * fabs(tmod) / M_PI;
}

double
Waveform::evalSaw(double tnorm) {
    double tmod = fmod(tnorm, 2.0 * M_PI) - M_PI;
    return tmod / M_PI;
}

double
Waveform::evalSqu(double tnorm) {
    double tmod = fmod(tnorm, 2.0 * M_PI);
    if (tmod < M_PI) {
        return 1.0;
    }
    return -1.0;
}

Waveform::Waveform(unsigned int _mode,
                   unsigned int _tableSize,
                   double *waveform,
                   double _fundamentalFrequency) {
    mode = _mode;
    tableSize = _tableSize;
    tableSizeFloat = (double)_tableSize;
    timeAtTrigger = 0.0;
    fundamentalFrequency = _fundamentalFrequency;

    if (mode == MODE_WAV) {
        waveTable = waveform;
    } else {
        waveTable = new double[tableSize];

        for (unsigned int indSample = 0; indSample < tableSize; indSample++) {
            double angle = ((double) indSample) * 2.0 * M_PI / tableSizeFloat;

            switch(mode) {
            case MODE_SIN:
                waveTable[indSample] = evalSin(angle);
                break;
            case MODE_TRI:
                waveTable[indSample] = evalTri(angle);
                break;
            case MODE_SAW:
                waveTable[indSample] = evalSaw(angle);
                break;
            case MODE_SQU:
                waveTable[indSample] = evalSqu(angle);
                break;
            }
        }
    }
}

void
Waveform::recreateWithTimbre(std::vector<double> &timbreAmplitudes,
                             std::vector<double> &timbreCoefficients) {
    assert(timbreAmplitudes.size() == timbreCoefficients.size());

    for (unsigned int indSample; indSample < tableSize; indSample++)
        waveTable[indSample] = 0.0;

    for (unsigned int indTimbre = 0; indTimbre < timbreAmplitudes.size(); indTimbre++) {

        for (unsigned int indSample = 0; indSample < tableSize; indSample++) {
            double angle = ((double) indSample) * 2.0 * timbreCoefficients[indTimbre] * M_PI / tableSizeFloat;

            switch(mode) {
            case MODE_SIN:
                waveTable[indSample] += evalSin(angle) * timbreAmplitudes[indTimbre];
                break;
            case MODE_TRI:
                waveTable[indSample] += evalTri(angle) * timbreAmplitudes[indTimbre];
                break;
            case MODE_SAW:
                waveTable[indSample] += evalSaw(angle) * timbreAmplitudes[indTimbre];
                break;
            case MODE_SQU:
                waveTable[indSample] += evalSqu(angle) * timbreAmplitudes[indTimbre];
                break;
            }
        }
    }
}


Waveform::~Waveform() {
    if (mode != MODE_WAV) {
        delete waveTable;
        waveTable = 0;
    }
}

Waveform::Waveform(const Waveform &waveform) {
    fundamentalFrequency = waveform.fundamentalFrequency;
    timeAtTrigger = waveform.timeAtTrigger;
    tableSizeFloat = waveform.tableSizeFloat;
    tableSize = waveform.tableSize;
    mode = waveform.mode;

    if (mode == MODE_WAV) {
        waveTable = waveform.waveTable;
    } else {
        waveTable = new double[tableSize];
        std::copy(waveform.waveTable, waveform.waveTable + tableSize, waveTable);
    }
}

Waveform&
Waveform::operator=(const Waveform &other) {
    if (&other == this) return *this;

    fundamentalFrequency = other.fundamentalFrequency;
    timeAtTrigger = other.timeAtTrigger;
    tableSizeFloat = other.tableSizeFloat;
    tableSize = other.tableSize;
    mode = other.mode;

    if (mode == MODE_WAV) {
        waveTable = other.waveTable;
    } else {
        waveTable = new double[tableSize];
        std::copy(other.waveTable, other.waveTable + tableSize, waveTable);
    }

    return *this;
}

double
Waveform::eval(double t) {
    double tmod = fmod((t - timeAtTrigger) / fundamentalFrequency, 2*M_PI);
    if (tmod < 0) tmod += 2*M_PI;

    // Position indexed by a continuous variable does not generally fall on
    // integer-valued points. Here indF is the "continuous-valued position"
    // of the argument and is somewhere between the integers qFloor(indF) and
    // qCeil(indF). When indF is not an integer, we use linear interpolation
    // to obtain the appropriate value.

    double indF = tableSizeFloat * tmod / (2.0 * M_PI);
    if (indF == tableSizeFloat) indF = 0.0;

    assert(indF >= 0.0);
    assert(indF < tableSizeFloat);

    unsigned int ind_min = (unsigned int) floor(indF);
    unsigned int ind_max = (unsigned int) ceil(indF);

    assert(ind_min <= ind_max);
    assert(ind_max <= tableSize);
    assert(ind_min < tableSize);

    double indmod = indF - (double)ind_min;
    assert(indmod < 1 && indmod >= 0);

    double value_next, value_prev;

    if (ind_min == ind_max) {
        return waveTable[ind_min];
    } else if (ind_max == tableSize) {
        value_prev = waveTable[ind_min];
        value_next = waveTable[0];
        return indmod * value_next + (1-indmod) * value_prev;
    } else if (ind_min < ind_max) {
        assert(ind_max < tableSize);

        value_prev = waveTable[ind_min];
        value_next = waveTable[ind_max];
        return indmod * value_next + (1-indmod) * value_prev;
    }
    return 0;
}

void
Waveform::trigger(double initialTime) {
    timeAtTrigger = initialTime;
}
