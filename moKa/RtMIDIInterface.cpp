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

#include "RtMIDIInterface.h"
#include <assert.h>

RtMIDIInterface::RtMIDIInterface() {
    midiIn = new RtMidiIn;
    nPorts = midiIn->getPortCount();

    std::string portName;
    for (unsigned int i=0; i<nPorts; i++) {
        try {
            portName = midiIn->getPortName(i);
        }
        catch (RtMidiError &error ) {
            error.printMessage();
        }
        portNames.push_back(portName);
        //std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
    }

    noteOffCallbackEnabled = false;
    noteOnCallbackEnabled = false;
    channelModeCallbackEnabled = false;
    noteOnUserData = 0;
    noteOffUserData = 0;
    activePort = 0;
    portOpen = false;
}

RtMIDIInterface::~RtMIDIInterface() {
    delete midiIn;
    midiIn = 0;
}

unsigned int
RtMIDIInterface::getCurrentPort() {
    return activePort;
}

bool
RtMIDIInterface::getPortOpen() {
    return portOpen;
}

unsigned int
RtMIDIInterface::getNumberOfPorts() {
    return nPorts;
}

std::string
RtMIDIInterface::getPortName(unsigned int port) {
    assert(port < nPorts);
}

void
RtMIDIInterface::openPort(unsigned int port) {
    midiIn->openPort(port);
    midiIn->ignoreTypes(false, false, false);
    midiIn->setCallback(&RtCallback, this);

    activePort = port;
    portOpen = true;
}

void
RtMIDIInterface::closePort() {
    assert(portOpen);

    midiIn->closePort();
    activePort = 0;
    portOpen = false;
}

void
RtMIDIInterface::setNoteOnCallback(noteOnCallback callback,
                                   void *userData) {
    _noteOnCallback = callback;
    noteOnCallbackEnabled = true;
    noteOnUserData = userData;
}

void
RtMIDIInterface::setNoteOffCallback(noteOffCallback callback,
                                    void *userData) {
    _noteOffCallback = callback;
    noteOffCallbackEnabled = true;
    noteOffUserData = userData;
}

void
RtMIDIInterface::setChannelModeCallback(channelModeCallback callback,
                                    void *userData) {
    _channelModeCallback = callback;
    channelModeCallbackEnabled = true;
    channelModeUserData = userData;
}


void
RtMIDIInterface::RtCallback(double deltaTime,
                            std::vector<unsigned char> *message,
                            void *userData) {
    RtMIDIInterface *iface = (RtMIDIInterface *) userData;

    unsigned int nBytes = message->size();

    for ( unsigned int i=0; i<nBytes; i++ )
        std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
    if ( nBytes > 0 )
        std::cout << "stamp = " << deltaTime << std::endl;

    if (nBytes >= 3)
    {
        unsigned char func = message->at(0),
                    data1= message->at(1),
                    data2= message->at(2);

        if (func >= 176 && func <= 179) {
            if (iface->channelModeCallbackEnabled) {
                unsigned char bank = func-176;
                (*iface->_channelModeCallback)(data1, data2, bank,  iface->channelModeUserData);
            }
        }

        if (func >= 128 && func <= 143) {
            unsigned char channel = func - 128;
            unsigned char note    = data1;

            if (iface->noteOffCallbackEnabled)
                (*iface->_noteOffCallback)(channel, note, iface->noteOffUserData);
        }
        if (func >= 144 && func <= 159) {
            unsigned char channel = func - 144;
            unsigned char note    = data1;
            unsigned char velocity= data2;

            if (velocity > 0) {
                if (iface->noteOnCallbackEnabled)
                   (*iface->_noteOnCallback)(channel, note, velocity, iface->noteOnUserData);
            } else {
                if (iface->noteOffCallbackEnabled)
                    (*iface->_noteOffCallback)(channel, note, iface->noteOffUserData);
            }
        }
    }
}
