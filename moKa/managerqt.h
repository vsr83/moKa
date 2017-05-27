#ifndef MANAGERQT_H
#define MANAGERQT_H

/*
 * The ManagerQt object handles the interaction between the audio interface,
 * MIDI interface, user interface and coordinates the high-level activities
 * of the audio synthesis.
 */

#include <QObject>
#include <QString>
#include <QList>
#include <QMutex>

#include "AudioInterface.h"
#include "RtMIDIInterface.h"

#include "patch.h"
#include "waveform.h"
#include "modulation.h"
#include "envelope.h"
#include "effect.h"
#include "kbWidget.h"

#include "freeverb/revmodel.hpp"

#include <vector>
#include <list>

class ManagerQt : public QObject {
   Q_OBJECT
public:
    ManagerQt();

   /*
    * The openMIDI procedure creates an instance of RtMIDIInterface and sets
    * noteOnCallback, noteOffCallback and channelModeCallback as callbacks
    * for the respective MIDI messages from the corresponding device. In
    * internal implementation of the RTMidi library, port numbers are not
    * unique but represent ordering of currently available MIDI devices.
    * However, at least under Linux and Alsa, the port names contain the
    * unique actual MIDI interface ID. Thus, the port names are used instead
    * of the IDs.
    */
    void openMIDI(std::string name);  // [In] Name of the MIDI Input port.
    void closeMIDI(std::string name); // [In] Name of the MIDI Input port.

   /*
    * The following two methods open and close the DEFAULT audio interface
    * and set the generateCallback as the callback function used for audio
    * synthesis:
    */
    void openAudioInterface();
    void closeAudioInterface();

private:
    AudioInterface *audioInterface;
    //RtMIDIInterface *MIDIInterface;

   /*
    * To each activce MIDI Input device, we associate a RtMIDIInterface
    * object and thus internally a RtMidiIn object. The selection of the
    * active devices is received to the MIDIIFSelectionStatus slot.
    */
    QList<RtMIDIInterface *> MIDIInterfaces;

    bool MIDIOpen(QString name);

    double getCurrentTime();

   /*
    * The high-level activities for the audio synthesis are carried out in the
    * method generateCallback, which is called by the audio interface whenever
    * new samples are needed. The parameters for each generated note are
    * contained in Patch objects, which are associated to each MIDI channel in
    * the activePatches vector. When an active note is created, a copy of the
    * patch associated to the MIDI channel of the note is created and added
    * to the list activeSounds[channel] with appropriately modified note and
    * amplitude parameters.
    */

    static void generateCallback(double t,                 // [In] Time from the initialization of the audio IF.
                                 double dt,                // [In] Duration of each sample.
                                 unsigned int numSamples,  // [In] Number of samples to be generated.
                                 float *outputLeft,        // [In] Pointer to the start of samples for left channel.
                                 float *outputRight,       // [In] Pointer to the start of samples for left channel.
                                 void *userData);          // [In] Pointer to the ManagerQt object.

   /*
    * Signals from the MIDI Input devices are received from each MIDI IF via
    * the following callback functions:
    */

    static void noteOnCallback(unsigned char channel,
                               unsigned char note,
                               unsigned char vel,
                               void *userData);            // [In] Pointer to the ManagerQt object.
    static void noteOffCallback(unsigned char channel,
                                unsigned char note,
                                void *userData);           // [In] Pointer to the ManagerQt object.
    static void channelModeCallback(unsigned char byte1,
                                    unsigned char byte2,
                                    unsigned char bank,
                                    void *userData);       // [In] Pointer to the ManagerQt object.

    // Default Patch, list of active patches and effects for each MIDI channel.
    std::vector<Patch> activePatches;
    std::vector<std::list<Patch> > activeSounds;
    std::vector<Effect> channelEffects;

    // Reverb model applied to all channels.
    bool reverbEnabled;
    revmodel reverbModel;

   /*
    * Since each MIDI Interfaces, the audio interface and the GUI run in
    * separate threads and manipulate the same objects in the ManagerQt
    * object, a mutex is needed:
    */

    QMutex mutex;

   /*
    * The following signals and slots contain the code for the interaction
    * with the GUI.
    */
public slots:
    void noteOn(unsigned char channel,
                unsigned char note,
                unsigned char vel);
    void noteOff(unsigned char channel,
                 unsigned char note);
    void setReverb(bool enabled,
                   double wet,
                   double roomSize);
   /*
    * Active MIDI interfaces are configured with the following method.
    */

    void MIDIIFSelectionStatus(const QVector<QString>      MIDIIFList,
                               const QVector<unsigned int> MIDIIFCurrentNumbers,
                               const QVector<bool>         MIDIIFSelectionStatus);
    void setChannelPatch(unsigned int MIDIChannel,
                         const Patch &patch);
signals:
    void initializeReverb(bool enabled,
                          double wet,
                          double reverb);

    void signalNoteOn(unsigned char channel,
                      unsigned char note,
                      unsigned char vel);
    void signalNoteOff(unsigned char channel,
                       unsigned char note);
    void signalChannelMode(unsigned char byte1,
                           unsigned char byte2,
                           unsigned char bank);
    void debugText(const QString &text);
};

#endif // MANAGERQT_H
