#include "managerqt.h"
#include <QTextStream>
#include <QDebug>

ManagerQt::ManagerQt() {
    audioInterface = 0;

    std::list<Patch> patchlist;
    Waveform   waveform(Waveform::MODE_SIN);
    Modulation modulation;

    modulation.FMfreqCoeff = 0.0;
    Envelope   envelope;
    Filter     filter(Filter::FILTER_LOWPASS, Filter::WINDOW_RECT, 256, 44100, 4000.0);
    Patch patch(waveform, envelope, modulation, filter);
    Effect     effect(44100, 44100*10, false);

    for (unsigned int channel = 0; channel < 16; channel++) {
        activeSounds.push_back(patchlist);
        activePatches.push_back(patch);
        if (channel == 0) {
            Effect     effectDelay(44100, 44100*10, false);
            patchEffects.push_back(effectDelay);

        } else {
            patchEffects.push_back(effect);
        }
        std::vector<double> mapping = {0.0, 0.0, 0.0, 0.0,  0.0, 0.0, 0.0, 0.0,
                                       0.0, 0.0, 0.0, 0.0,  0.0, 0.0, 0.0, 0.0};
        mapping[channel] = 1.0;
        channelPatchMapping.push_back(mapping);
    }

    std::vector<double> mapping = {0.0, 0.0, 0.6, 0.3,  0.3, 0.0, 0.0, 0.0,
                                   0.0, 0.0, 0.0, 0.0,  0.0, 0.0, 0.0, 0.0};
    channelPatchMapping[7] = mapping;
    reverbEnabled = true;
    reverbModel.setroomsize(0.75f);
    reverbModel.setwet(0.7f);

    lastMIDIChannel = 0;
}

void
ManagerQt::openAudioInterface() {
    audioInterface = new AudioInterface(44100, 64);
    audioInterface->setGenerateCallback(&ManagerQt::generateCallback,
                                        this);
    audioInterface->openStream();
}

void
ManagerQt::closeAudioInterface() {
    Q_ASSERT(audioInterface != 0);

    audioInterface->closeStream();
    delete audioInterface;
    audioInterface = 0;
}


void
ManagerQt::openMIDI(std::string name) {
    RtMIDIInterface *MIDIInterface;

    MIDIInterface = new RtMIDIInterface();
    MIDIInterface->openPort(name);
    MIDIInterface->setNoteOnCallback(&noteOnCallback, this);
    MIDIInterface->setNoteOffCallback(&noteOffCallback, this);
    MIDIInterface->setChannelModeCallback(&channelModeCallback, this);

    MIDIInterfaces.push_back(MIDIInterface);
}

void
ManagerQt::closeMIDI(std::string name) {
    for (QList<RtMIDIInterface*>::iterator it = MIDIInterfaces.begin(); it != MIDIInterfaces.end();) {
        RtMIDIInterface *IF;

        IF = *it;
        if (IF->getCurrentPortName() == name) {
            it = MIDIInterfaces.erase(it);
            IF->closePort();
            delete IF;
        } else {
            it++;
        }
    }
}

double
ManagerQt::getCurrentTime() {
    Q_ASSERT(audioInterface != 0);

    return audioInterface->getCurrentTime();
}

void
ManagerQt::generateCallback(double t,
                                 double dt,
                                 unsigned int numSamples,
                                 float *outputLeft,
                                 float *outputRight,
                                 void *userData) {
    ManagerQt *manager = (ManagerQt *) userData;
    manager->mutex.lock();

    float *outputMono = new float[numSamples];

    for (unsigned int sample = 0; sample < numSamples; sample++) {
        outputMono[sample] = 0.0;
    }

    double currentTime = manager->getCurrentTime();

    for (unsigned int patch = 0; patch < manager->activePatches.size(); patch++) {
        float *patchOutput = new float[numSamples];

        for (unsigned int sample = 0; sample < numSamples; sample++) {
            patchOutput[sample] = 0.0;
        }

        for (unsigned int channel = 0; channel < 16; channel++) {
            for (std::list<Patch>::iterator it = manager->activeSounds[channel].begin(); it != manager->activeSounds[channel].end();) {

                if ((*it).index == patch) {
                    if ((*it).isFinished()) {
                        manager->activeSounds[channel].erase(it++);
                    } else {
                        double t = currentTime;

                        for (unsigned int sample = 0; sample < numSamples; sample++) {
                            patchOutput[sample] += 0.025 * (*it).eval(t);
                            t += dt;
                        }
                        ++it;
                    }
                } else {
                    ++it;
                }
            }
        }

        manager->patchEffects[patch].filterRBAddData(patchOutput, numSamples);
        if (manager->activePatches[patch].hasFilter()) {
            manager->patchEffects[patch].filterConvolve(&manager->activePatches[patch].filter,
                                                        patchOutput,
                                                        numSamples);
            if (manager->patchEffects[patch].delayEnabled) {
                manager->patchEffects[patch].delayRBAddData(patchOutput,
                                                            numSamples);

                manager->patchEffects[patch].delayRBapply(patchOutput,
                                                            numSamples);
            }

        }

        for (unsigned int sample = 0; sample < numSamples; sample++) {
            outputMono[sample] += patchOutput[sample];
        }
        delete [] patchOutput;
        patchOutput = 0;
    }

    if (manager->reverbEnabled) {
        manager->reverbModel.processreplace(outputMono, outputMono, outputLeft, outputRight, numSamples, 1);
    } else {
        std::copy(outputMono, outputMono + numSamples, outputLeft);
        std::copy(outputMono, outputMono + numSamples, outputRight);
    }
    delete[] outputMono;
    manager->mutex.unlock();
}

void
ManagerQt::noteOnCallback(unsigned char channel,
                          unsigned char note,
                          unsigned char vel,
                          void *userData) {
    ManagerQt *manager = (ManagerQt *) userData;

    manager->mutex.lock();
    manager->lastMIDIChannel = channel;

    for (unsigned int indPatch = 0; indPatch < manager->activePatches.size(); indPatch++) {
        if (manager->channelPatchMapping[channel][indPatch] != 0.0) {
            Patch patch = manager->activePatches[indPatch];
            patch.triggerFromMIDI(note, vel, manager->getCurrentTime());
            patch.amplitudeCoeff = manager->channelPatchMapping[channel][indPatch];
            manager->activeSounds[channel].push_back(patch);
        }
    }
    emit manager->signalNoteOn(channel, note, vel);

    QString debugStr;
    QTextStream(&debugStr) << "MIDI - NOTE ON ch=" << channel
                           << ", note = " << note
                           << ", vel = " << vel;

    emit manager->debugText(debugStr);

    manager->mutex.unlock();
}

void
ManagerQt::noteOffCallback(unsigned char channel,
                           unsigned char note,
                           void *userData) {
    ManagerQt *manager = (ManagerQt *) userData;
    manager->mutex.lock();

    for (std::list<Patch>::iterator it = manager->activeSounds[channel].begin(); it != manager->activeSounds[channel].end(); it++) {
        if ((*it).note == note)
            (*it).release(manager->getCurrentTime());
    }

    emit manager->signalNoteOff(channel, note);

    QString debugStr;
    QTextStream(&debugStr) << "MIDI - NOTE OFF ch=" << channel
                           << ", note = " << note;

    emit manager->debugText(debugStr);
    manager->mutex.unlock();
}

void
ManagerQt::channelModeCallback(unsigned char byte1,
                               unsigned char byte2,
                               unsigned char bank,
                               void *userData) {
    ManagerQt *manager = (ManagerQt *) userData;
    manager->mutex.lock();

    emit manager->signalChannelMode(byte1, byte2, bank);

    QString debugStr;
    QTextStream(&debugStr) << "MIDI - CHANNEL MODE byte1=" << byte1
                           << ", byte2 = " << byte2
                           << ", bank = "  << bank;

    Q_ASSERT(manager->lastMIDIChannel < 16);

    if (byte1 >= 12 && byte1 <= 19) { // sliders 1-8
        int knob = byte1 - 12;
        double value = ((float)byte2)/127.0;

        if (bank == 0) {
            manager->channelPatchMapping[manager->lastMIDIChannel][knob] = ((float)byte2)/127.0;
        } else if (bank == 1) {
            manager->channelPatchMapping[manager->lastMIDIChannel][knob + 8] = ((float)byte2)/127.0;
        }
    }

    emit manager->debugText(debugStr);
    manager->mutex.unlock();
}

void
ManagerQt::noteOn(unsigned char channel,
                  unsigned char note,
                  unsigned char vel) {
    noteOnCallback(channel, note, vel, this);
}

void
ManagerQt::noteOff(unsigned char channel,
                   unsigned char note) {
    noteOffCallback(channel, note, this);
}

void
ManagerQt::setReverb(bool enabled,
                     double wet,
                     double roomSize) {
    reverbModel.setwet(wet);
    reverbModel.setroomsize(roomSize);
    reverbEnabled = enabled;
}

void
ManagerQt::MIDIIFSelectionStatus(const QVector<QString>      MIDIIFList,
                                 const QVector<unsigned int> MIDIIFCurrentNumbers,
                                 const QVector<bool>         MIDIIFSelection) {
    Q_ASSERT(MIDIIFList.size() == MIDIIFCurrentNumbers.size());
    Q_ASSERT(MIDIIFList.size() == MIDIIFSelection.size());

    mutex.lock();
    // Check for disconnected devices:
    for (QList<RtMIDIInterface*>::iterator it = MIDIInterfaces.begin(); it != MIDIInterfaces.end();it++) {
        RtMIDIInterface *IF = *it;

        QString portName = QString::fromStdString(IF->getCurrentPortName());
        int listIndex = MIDIIFList.indexOf(portName);

        if (listIndex == -1) {
            qDebug() << "Closing " << portName;
            closeMIDI(portName.toStdString());
            qDebug() << "Ok";
        }
    }


    for (unsigned int indIF = 0; indIF < MIDIIFList.size(); indIF++) {
        QString      portName    = MIDIIFList[indIF];
        unsigned int portNumber  = MIDIIFCurrentNumbers[indIF];
        bool         portEnabled = MIDIIFSelection[indIF];

        qDebug() << portName << " " << portNumber << " " << portEnabled;

        if (portEnabled && !MIDIOpen(portName)) {
            qDebug() << "Opening " << portName;
            openMIDI(portName.toStdString());
            qDebug() << "Ok";
        }
        if (!portEnabled && MIDIOpen(portName)) {
            qDebug() << "Closing " << portName;
            closeMIDI(portName.toStdString());
            qDebug() << "Ok";
        }
    }
    mutex.unlock();
}

bool
ManagerQt::MIDIOpen(QString name) {
    qDebug() << "MIDIOpen";
    for (auto it : MIDIInterfaces) {
    //for (QList<RtMIDIInterface*>::iterator it = MIDIInterfaces.begin(); it != MIDIInterfaces.end();it++) {
        RtMIDIInterface *IF = it;

        qDebug() << "iter";

        if (IF->getCurrentPortName() == name.toStdString()){
            qDebug() << "MIDIOpen - True";
            return true;
        }
    }
    qDebug() << "MIDIOpen - False";
    return false;
}

void
ManagerQt::setPatch(unsigned int index,
                    const Patch &patch) {
    Q_ASSERT(index < 16);
    mutex.lock();
    emit debugText(tr("Setting patch"));
    activePatches[index] = patch;
    activePatches[index].index = index;
    mutex.unlock();
}

