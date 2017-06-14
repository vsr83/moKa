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
    Patch(Waveform   &_waveform,
          Envelope   &_envelope,
          Modulation &_modulation,
          Filter     &_filter);
    Patch();
    ~Patch();

    Waveform getWaveform();
    void setWaveform(Waveform &_waveform);
    void setEnvelope(Envelope &_envelope);
    void setTimbre(std::vector<double> &_timbreAmplitudes,
                   std::vector<double> &_timbreCoefficients);

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
    bool hasFilter();

    unsigned char note;
    unsigned char vel;

    unsigned int index;

    Filter     filter;
    double amplitudeCoeff;
private:
    double frequency;
    double amplitude;
    double initialTime;
    double phaseShift;

    Waveform   waveform;
    Envelope   envelope;
    Modulation modulation;

    // External (not in waveform) Timbre
    std::vector<double> timbreAmplitudes;
    std::vector<double> timbreCoefficients;
};

#endif // PATCH_H

