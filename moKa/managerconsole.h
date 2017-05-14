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

#ifndef MANAGERCONSOLE_H
#define MANAGERCONSOLE_H

#include "AudioInterface.h"
#include "RtMIDIInterface.h"

#include "patch.h"
#include "waveform.h"
#include "modulation.h"
#include "envelope.h"

#include <vector>
#include <list>

class ManagerConsole
{
public:
    ManagerConsole();

    void openMIDI(unsigned int port);
    void openAudioInterface();
    void closeMIDI();
    void closeAudioInterface();

private:
    AudioInterface *audioInterface;
    RtMIDIInterface *MIDIInterface;

    double getCurrentTime();

    static void generateCallback(double t,
                                 double dt,
                                 unsigned int numSamples,
                                 float *outputLeft,
                                 float *outputRight,
                                 void *userData);

    static void noteOnCallback(unsigned char channel,
                               unsigned char note,
                               unsigned char vel,
                               void *userData);
    static void noteOffCallback(unsigned char channel,
                                unsigned char note,
                                void *userData);
    static void channelModeCallback(unsigned char byte1,
                                    unsigned char byte2,
                                    unsigned char bank,
                                    void *userData);

    std::vector<Patch> activePatches;
    std::vector<std::list<Patch> > activeSounds;
};

#endif // MANAGERCONSOLE_H
