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

Patch::Patch(Waveform &_waveform,
             Envelope &_envelope,
             Modulation &_modulation) {
    waveform   = _waveform;
    envelope   = _envelope;
    modulation = _modulation;

    frequency = 0.0;
    amplitude = 0.0;
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
    double envelopeValue = 1.0;
    double modulatedTime = modulation.eval((t - initialTime) * 2 * M_PI * frequency, amplitude);

    //modulatedTime = (t - initialTime);

    double out = waveform.eval(modulatedTime);

   // std::cout << out << std::endl;
    if (withEnvelope)
        out *= envelope.eval(t);

    //out = amplitude * sin(frequency * (t - initialTime));

    //std::cout << t - initialTime << " " << out << std::endl;

    return out;
}
