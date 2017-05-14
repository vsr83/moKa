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

#ifndef PATCH_H
#define PATCH_H

#include "envelope.h"
#include "waveform.h"
#include "modulation.h"
#include "filter.h"

class Patch {
public:
    Patch(Waveform &_waveform,
          Envelope &_envelope,
          Modulation &_modulation);
    ~Patch();

    void trigger(double _frequency,
                 double _amplitude,
                 double _initialTime);
    void triggerFromMIDI(unsigned char _note,
                         unsigned char _vel,
                         double _initialTime);
    void release(double t);
    bool isFinished();

    double eval(double t,
                bool withEnvelope = true);

    unsigned char note;
    unsigned char vel;
private:
    double frequency;
    double amplitude;
    double initialTime;

    Waveform   waveform;
    Envelope   envelope;
    Modulation modulation;
    Filter     filter;
};

#endif // PATCH_H
