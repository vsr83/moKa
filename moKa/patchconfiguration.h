#ifndef PATCHCONFIGURATION_H
#define PATCHCONFIGURATION_H

#include <QWidget>
#include "patch.h"
#include "modulation.h"
#include "waveform.h"
#include "effect.h"

class PatchConfiguration : public QWidget {
    Q_OBJECT
public:
    PatchConfiguration(QWidget *parent = 0);
    void configureDefaultPatches();
private:
signals:
    void setChannelPatch(unsigned int MIDIChannel,
                         const Patch &patch);
};

#endif // PATCHCONFIGURATION_H
