#ifndef PATCHCONFIGURATION_H
#define PATCHCONFIGURATION_H

#include <QWidget>
#include <QGridLayout>
#include <vector>

#include "patch.h"
#include "modulation.h"
#include "waveform.h"
#include "effect.h"
#include "adsrwidget.h"
#include "timbrewidget.h"

class PatchConfiguration : public QWidget {
    Q_OBJECT
public:
    PatchConfiguration(unsigned int numPatches,
                       QWidget *parent = 0);
    void configureDefaultPatches();
private:
    QGridLayout *gridLayout;
    ADSRWidget  *adsrWidget;
    TimbreWidget *internalTimbre;
    TimbreWidget *externalTimbre;

    std::vector<Patch> patchConfiguration;

    void configurePatch(unsigned int patchNumber,
                        const Patch &patch);

    unsigned int currentPatch;
public slots:
    void setInternalTimbre(QVector<double> &amplitudes,
                           QVector<double> &coefficients);
    void setExternalTimbre(QVector<double> &amplitudes,
                           QVector<double> &coefficients);
    void envelopeChanged(Envelope &envelope);
signals:
    void setPatch(unsigned int MIDIChannel,
                  const Patch &patch);
};

#endif // PATCHCONFIGURATION_H
