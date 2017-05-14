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

#include "managerconsole.h"
#include <assert.h>
#include <algorithm>

ManagerConsole::ManagerConsole() {
    audioInterface = 0;
    MIDIInterface = 0;

    for (unsigned int channel = 0; channel < 16; channel++) {
        std::list<Patch> patchlist;
        Waveform   waveform(Waveform::MODE_SAW);
        Modulation modulation;
        Envelope   envelope;
        Filter     filter(Filter::FILTER_LOWPASS, Filter::WINDOW_RECT, 32, 44100, 1000.0);
        Patch patch(waveform, envelope, modulation, filter);
        Effect     effect;

        activeSounds.push_back(patchlist);
        activePatches.push_back(patch);
        channelEffects.push_back(effect);
    }
}

void
ManagerConsole::openAudioInterface() {
    audioInterface = new AudioInterface(44100, 64);
    audioInterface->setGenerateCallback(&ManagerConsole::generateCallback,
                                        this);
    audioInterface->openStream();
}

void
ManagerConsole::closeAudioInterface() {
    assert(audioInterface != 0);

    audioInterface->closeStream();
    delete audioInterface;
    audioInterface = 0;
}


void
ManagerConsole::openMIDI(unsigned int port) {
    assert(MIDIInterface == 0);

    MIDIInterface = new RtMIDIInterface();
    MIDIInterface->openPort(port);
    MIDIInterface->setNoteOnCallback(&noteOnCallback, this);
    MIDIInterface->setNoteOffCallback(&noteOffCallback, this);
    MIDIInterface->setChannelModeCallback(&channelModeCallback, this);
}

void
ManagerConsole::closeMIDI() {
    assert(MIDIInterface != 0);

    MIDIInterface->closePort();
    delete MIDIInterface;
    MIDIInterface = 0;
}

double
ManagerConsole::getCurrentTime() {
    assert(audioInterface != 0);

    return audioInterface->getCurrentTime();
}

void
ManagerConsole::generateCallback(double t,
                                 double dt,
                                 unsigned int numSamples,
                                 float *outputLeft,
                                 float *outputRight,
                                 void *userData) {
    ManagerConsole *manager = (ManagerConsole *) userData;

    float *outputMono = new float[numSamples];

    for (unsigned int sample = 0; sample < numSamples; sample++) {
        outputMono[sample] = 0.0;
    }

    double currentTime = manager->getCurrentTime();

    for (unsigned int channel = 0; channel < 16; channel++) {
        float *channelOutput = new float[numSamples];
        for (unsigned int sample = 0; sample < numSamples; sample++) {
            channelOutput[sample] = 0.0;
        }

        for (std::list<Patch>::iterator it = manager->activeSounds[channel].begin(); it != manager->activeSounds[channel].end();) {
            if ((*it).isFinished()) {
                manager->activeSounds[channel].erase(it++);
            } else {
                double t = currentTime;

                for (unsigned int sample = 0; sample < numSamples; sample++) {
                    channelOutput[sample] += 0.1 * (*it).eval(t);
                    t += dt;
                }
                ++it;
            }
        }
        manager->channelEffects[channel].filterRBAddData(channelOutput, numSamples);
        if (manager->activePatches[channel].hasFilter()) {
            manager->channelEffects[channel].filterConvolve(&manager->activePatches[channel].filter,
                                                            channelOutput,
                                                            numSamples);
        }

        for (unsigned int sample = 0; sample < numSamples; sample++) {
            outputMono[sample] += channelOutput[sample];
        }
        delete [] channelOutput;
        channelOutput = 0;
    }

    std::copy(outputMono, outputMono + numSamples, outputLeft);
    std::copy(outputMono, outputMono + numSamples, outputRight);

    delete[] outputMono;
}

void
ManagerConsole::noteOnCallback(unsigned char channel,
                               unsigned char note,
                               unsigned char vel,
                               void *userData) {
    ManagerConsole *manager = (ManagerConsole *) userData;

    Patch patch = manager->activePatches[channel];
    patch.triggerFromMIDI(note, vel, manager->getCurrentTime());
    manager->activeSounds[channel].push_back(patch);
}

void
ManagerConsole::noteOffCallback(unsigned char channel,
                                unsigned char note,
                                void *userData) {
    ManagerConsole *manager = (ManagerConsole *) userData;

    for (std::list<Patch>::iterator it = manager->activeSounds[channel].begin(); it != manager->activeSounds[channel].end(); it++) {
        if ((*it).note == note)
            (*it).release(manager->getCurrentTime());
    }
}

void
ManagerConsole::channelModeCallback(unsigned char byte1,
                                    unsigned char byte2,
                                    unsigned char bank,
                                    void *userData) {
    ManagerConsole *manager = (ManagerConsole *) userData;

}
