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

#include "patch.h"
#include <math.h>
#include <iostream>
#include <assert.h>

Patch::Patch(Waveform &_waveform,
             Envelope &_envelope,
             Modulation &_modulation,
             Filter     &_filter) {
    waveform   = _waveform;
    envelope   = _envelope;
    modulation = _modulation;
    filter     = _filter;

    frequency = 0.0;
    amplitude = 0.0;
    amplitudeCoeff = 1.0;

    timbreAmplitudes = {1.0};
    timbreCoefficients = {1.0};

    if (waveform.getMode() == Waveform::MODE_SIN) {
        phaseShift = ((double)rand()) / ((double)RAND_MAX);
    } else {
        phaseShift = 0;
    }

    index = 0;
}

Patch::Patch() {
    Waveform   _waveform(Waveform::MODE_SIN);
    Modulation _modulation;
    Envelope   _envelope;
    Filter     _filter(Filter::FILTER_LOWPASS, Filter::WINDOW_RECT, 256, 44100, 4000.0);

    waveform   = _waveform;
    envelope   = _envelope;
    modulation = _modulation;
    filter     = _filter;

    frequency = 0.0;
    amplitudeCoeff = 1.0;
    amplitude = 0.0;

    timbreAmplitudes = {1.0};
    timbreCoefficients = {1.0};
    phaseShift = 0;
    index = 0;
}

Patch::~Patch() {

}

void
Patch::trigger(double _frequency,
               double _amplitude,
               double _initialTime) {
    initialTime = _initialTime;
    frequency   = _frequency;
    amplitude   = _amplitude;

    envelope.trigger(initialTime);
    waveform.trigger(initialTime);
}

void
Patch::triggerFromMIDI(unsigned char _note,
                       unsigned char _vel,
                       double _initialTime) {
    note = _note;
    vel = _vel;

    frequency = 8.175 * powf(2, (((double)note)/12.0));
    amplitude = 1.0 * ((double)vel) / 127.0;

    std::cout << "Freq = " << frequency << std::endl;
    trigger(frequency, amplitude, _initialTime);
}

void
Patch::release(double t) {
    envelope.release(t);
}

bool
Patch::isFinished() {
    return envelope.isFinished();
}

double
Patch::eval(double t,
            bool withEnvelope) {
    assert(timbreAmplitudes.size() == timbreCoefficients.size());

    double envelopeValue = envelope.eval(t);
    double patchTime = t - initialTime;
    double out = 0.0;

    // Perform linear and / or FM synthesis:
    for (unsigned int indTimbre = 0; indTimbre < timbreAmplitudes.size(); indTimbre++) {
        double modulatedTime = modulation.eval(frequency * timbreCoefficients[indTimbre],
                                               patchTime,
                                               amplitude * timbreAmplitudes[indTimbre],
                                               envelopeValue);
        double coeff = 1.0;
        //if (indTimbre != 0) {
        //    coeff = exp(-patchTime * 10.0);
        //}

        out += amplitudeCoeff * timbreAmplitudes[indTimbre] * waveform.eval(modulatedTime + phaseShift) * coeff;
    }

    if (withEnvelope)
        out *= envelopeValue;
    return out;
}

bool
Patch::hasFilter() {
    return filter.type != Filter::FILTER_OFF;
}

void
Patch::setTimbre(std::vector<double> &_timbreAmplitudes,
                 std::vector<double> &_timbreCoefficients) {
    assert(_timbreAmplitudes.size() == _timbreCoefficients.size());

    timbreAmplitudes = _timbreAmplitudes;
    timbreCoefficients = _timbreCoefficients;
}

void
Patch::setEnvelope(Envelope &_envelope) {
    envelope = _envelope;
}

Waveform
Patch::getWaveform() {
    return waveform;
}

void
Patch::setWaveform(Waveform &_waveform) {
    waveform = _waveform;
}

/*
void
Patch::setInternalTimbre(std::vector<double> &_timbreAmplitudes,
                         std::vector<double> &_timbreCoefficients) {
    assert(_timbreAmplitudes.size() == _timbreCoefficients.size());
    waveform.recreateWithTimbre(_timbreAmplitudes,
                                _timbreCoefficients);
}
*/
