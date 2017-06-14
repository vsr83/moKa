#include "patchconfiguration.h"
#include <QGroupBox>
#include <QHBoxLayout>

PatchConfiguration::PatchConfiguration(unsigned int numPatches, QWidget *parent) : QWidget(parent) {
    gridLayout = new QGridLayout;
    QWidget *gridWidget = new QWidget();

    setLayout(gridLayout);
    setWindowTitle("Patch Configuration");

    adsrWidget = new ADSRWidget;
    QGroupBox *envelopeGroup = new QGroupBox(tr("Envelope"));
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(adsrWidget);
    envelopeGroup->setLayout(hbox);
    gridLayout->addWidget(envelopeGroup, 0, 0, 1, 2);

    QGroupBox *intTimbreGroup = new QGroupBox(tr("Internal Timbre"));
    QGroupBox *extTimbreGroup = new QGroupBox(tr("External Timbre"));
    QHBoxLayout *hbox2 = new QHBoxLayout;
    QHBoxLayout *hbox3 = new QHBoxLayout;

    internalTimbre = new TimbreWidget(8);
    externalTimbre = new TimbreWidget(8);
    hbox2->addWidget(internalTimbre);
    hbox3->addWidget(externalTimbre);
    intTimbreGroup->setLayout(hbox2);
    extTimbreGroup->setLayout(hbox3);

    gridLayout->addWidget(envelopeGroup, 0, 0, 1, 2);
    gridLayout->addWidget(intTimbreGroup, 1, 0, 1, 1);
    gridLayout->addWidget(extTimbreGroup, 1, 1, 1, 1);

    for (unsigned int indPatch = 0; indPatch < numPatches; indPatch++) {
        Patch tmpPatch;
        patchConfiguration.push_back(tmpPatch);
    }
    currentPatch = 0;
    connect(internalTimbre, SIGNAL(settingsChanged(QVector<int>&,QVector<int>&)),
            this,           SLOT(setInternalTimbre(QVector<int>&,QVector<int>&)));
    connect(externalTimbre, SIGNAL(settingsChanged(QVector<int>&,QVector<int>&)),
            this,           SLOT(setExternalTimbre(QVector<int>&,QVector<int>&)));
    connect(adsrWidget, SIGNAL(envelopeChanged(Envelope&)),
            this,       SLOT(envelopeChanged(Envelope&)));
}

void
PatchConfiguration::configurePatch(unsigned int indPatch,
                                   const Patch &patch) {
    Q_ASSERT(indPatch < patchConfiguration.size());
    patchConfiguration[indPatch] = patch;
    emit setPatch(indPatch, patch);

}

void
PatchConfiguration::envelopeChanged(Envelope &envelope) {
    Q_ASSERT(currentPatch < patchConfiguration.size());

    Patch patch = patchConfiguration[currentPatch];
    patch.setEnvelope(envelope);
    emit setPatch(currentPatch, patch);
}

void
PatchConfiguration::setInternalTimbre(QVector<int> &amplitudes,
                                      QVector<int> &phases) {
    Q_ASSERT(currentPatch < patchConfiguration.size());

    std::vector<double> timbreCoefficients;
    std::vector<double> timbreAmplitudes;

    Patch patch = patchConfiguration[currentPatch];
    Waveform waveform = patch.getWaveform();

    for (unsigned int indTimbre = 0; indTimbre < amplitudes.size(); indTimbre++) {
        timbreCoefficients.push_back((double)(indTimbre) + 1.0);
        timbreAmplitudes.push_back(0.01 * ((double)amplitudes[indTimbre]));
    }
    waveform.recreateWithTimbre(timbreAmplitudes, timbreCoefficients);
    patch.setWaveform(waveform);
    patchConfiguration[currentPatch] = patch;
    emit setPatch(currentPatch, patch);
}

void
PatchConfiguration::setExternalTimbre(QVector<int> &amplitudes,
                                      QVector<int> &phases) {
    Q_ASSERT(currentPatch < patchConfiguration.size());

    std::vector<double> timbreCoefficients;
    std::vector<double> timbreAmplitudes;

    Patch patch = patchConfiguration[currentPatch];

    for (unsigned int indTimbre = 0; indTimbre < amplitudes.size(); indTimbre++) {
        timbreCoefficients.push_back((double)(indTimbre) + 1.0);
        timbreAmplitudes.push_back(0.01 * ((double)amplitudes[indTimbre]));
    }
    timbreCoefficients[0] = 1.0;
    timbreCoefficients[1] = 0.999;
    timbreCoefficients[2] = 1.001;
    timbreCoefficients[3] = 0.99;
    timbreCoefficients[4] = 1.01;
    timbreCoefficients[5] = 0.95;
    timbreCoefficients[6] = 1.05;
    timbreCoefficients[7] = 2.0;
    patch.setTimbre(timbreAmplitudes, timbreCoefficients);
    patchConfiguration[currentPatch] = patch;
    emit setPatch(currentPatch, patch);
}

void
PatchConfiguration::configureDefaultPatches() {
    Waveform   WFSin(Waveform::MODE_SIN);
    Waveform   WFTri(Waveform::MODE_TRI);
    Waveform   WFSaw(Waveform::MODE_SAW);
    Waveform   WFSqu(Waveform::MODE_SQU);

    Modulation noModulation;
    noModulation.FMmode = Modulation::FM_OFF;
    Envelope   defaultEnv;
    defaultEnv.setMode(Envelope::MODE_EXP,
                       Envelope::MODE_EXP,
                       Envelope::MODE_EXP);
    defaultEnv.sustainAmplitude = 0.5;

    Filter     filterLP(Filter::FILTER_LOWPASS,
                        Filter::WINDOW_RECT,
                        256,
                        44100,
                        8000.0);
    std::vector<double> timbreAmplQuintadena8 = {0.3981072,
                                                 0.0251189,
                                                 1.0,
                                                 0.0251189,
                                                 0.0375837,
                                                 0.0281838,
                                                 0.0112202,
                                                 0.0112202,
                                                 0.0031623,
                                                 0.0112202};
    std::vector<double>timbreCoeffQuintadena8 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    Waveform WFQuintadena8Int = WFSin;
    WFQuintadena8Int.recreateWithTimbre(timbreAmplQuintadena8,
                                        timbreCoeffQuintadena8);
    Patch patchQuintadena8(WFQuintadena8Int, defaultEnv, noModulation, filterLP);

    std::vector<double> timbreAmplSubbas16 = {1.0,
                                              0.0223872,
                                              0.0891251,
                                              0.0223872,
                                              0.0014125,
                                              0.0079433,
                                              0.0039811,
                                              0.0079433,
                                              0.0015849,
                                              0.0039811};
    std::vector<double>timbreCoeffSubbas16 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    Waveform WFSubbas16 = WFSin;
    WFSubbas16.recreateWithTimbre(timbreAmplSubbas16,
                                  timbreCoeffSubbas16);
    Patch patchSubbas16(WFSubbas16, defaultEnv, noModulation, filterLP);

    std::vector<double> timbreAmplPrincipal8 = {1.0,
                                                0.7087858,
                                                0.3166030,
                                                0.0708786,
                                                0.0563009,
                                                0.355234,
                                                0.0089231,
                                                0.0141421,
                                                0.0100119,
                                                0.0447214};
    std::vector<double>timbreCoeffPrincipal8 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    Waveform WFPrincipal8Int = WFSin;
    WFPrincipal8Int.recreateWithTimbre(timbreAmplPrincipal8,
                                       timbreCoeffPrincipal8);
    Patch patchPrincipal8(WFPrincipal8Int, defaultEnv, noModulation, filterLP);

    std::vector<double> detuneAmpl = {0.75, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.125, 0.125};
    std::vector<double> detuneCoeff = {1.0, 1.05, 1.025, 0.95, 0.975, 2.025, 2.0125, 0.5125, 0.525};

    Patch patchQuintadena8Detuned = patchQuintadena8;
    patchQuintadena8Detuned.setTimbre(detuneAmpl, detuneCoeff);

    Patch patchSubbas16Detuned = patchSubbas16;
    patchSubbas16Detuned.setTimbre(detuneAmpl, detuneCoeff);

    Patch patchPrincipal8Detuned = patchPrincipal8;
    patchPrincipal8Detuned.setTimbre(detuneAmpl, detuneCoeff);

    Envelope   trumpetEnv;
    trumpetEnv.sustainAmplitude = 0.2;
    Modulation modulationTrumpet;
    modulationTrumpet.FMfreqCoeff = 0.5;
    modulationTrumpet.FMampl = 8.0;
    modulationTrumpet.FMmode = Modulation::FM_PROP_FREQ;
    Patch patchTrumpet(WFSin, defaultEnv, modulationTrumpet, filterLP);

    Modulation modulationFlute;
    modulationFlute.FMfreqCoeff = 2.0;
    modulationFlute.FMampl = 2.0;
    modulationFlute.FMmode = Modulation::FM_PROP_FREQ;
    Patch patchFlute(WFSin, defaultEnv, modulationFlute, filterLP);

    std::vector<double>timbreCoeffPrincipal4 = {0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5,6.0};
    std::vector<double>timbreCoeffPrincipal16 = {2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0,210.0};


    Envelope   defaultEnv4 = defaultEnv;
    Envelope   defaultEnv16 = defaultEnv;

    defaultEnv4.attackTime = defaultEnv.attackTime * 2;
    defaultEnv4.decayTime = defaultEnv.decayTime * 2;
    defaultEnv4.releaseTime = defaultEnv.releaseTime * 2;
    defaultEnv16.attackTime = defaultEnv.attackTime * 0.5;
    defaultEnv16.decayTime = defaultEnv.decayTime * 0.5;
    defaultEnv16.releaseTime = defaultEnv.releaseTime * 0.5;

    Waveform WFPrincipal4Int = WFSin;
    Waveform WFPrincipal16Int = WFSin;
    Waveform WFQuintadena4Int = WFSin;
    Waveform WFSubbas8Int = WFSin;
    WFPrincipal16Int.recreateWithTimbre(timbreAmplPrincipal8, timbreCoeffPrincipal16);
    WFPrincipal4Int.recreateWithTimbre(timbreAmplPrincipal8,  timbreCoeffPrincipal4);
    WFSubbas8Int.recreateWithTimbre(timbreAmplSubbas16,  timbreCoeffPrincipal4);
    WFQuintadena4Int.recreateWithTimbre(timbreAmplQuintadena8,  timbreCoeffPrincipal4);

    Patch patchPrincipal16(WFPrincipal16Int, defaultEnv16, noModulation, filterLP);
    Patch patchPrincipal4(WFPrincipal4Int, defaultEnv4, noModulation, filterLP);
    Patch patchSubbas8(WFSubbas8Int, defaultEnv4, noModulation, filterLP);
    Patch patchQuintadena4(WFQuintadena4Int, defaultEnv4, noModulation, filterLP);


    patchQuintadena4.setWaveform(WFSaw);

    configurePatch(0, patchQuintadena4);
    configurePatch(1, patchQuintadena8);
    configurePatch(2, patchSubbas8);
    configurePatch(3, patchSubbas16);
    configurePatch(4, patchPrincipal4);
    configurePatch(5, patchPrincipal8);
    configurePatch(6, patchPrincipal16);
    //emit setChannelPatch(5, patchQuintadena8Detuned);
    //emit setChannelPatch(6, patchSubbas16Detuned);
    //emit setChannelPatch(7, patchPrincipal8Detuned);
    configurePatch(7, patchTrumpet);
    configurePatch(8, patchFlute);
    configurePatch(9, patchQuintadena8Detuned);
    configurePatch(10, patchSubbas16Detuned);
    configurePatch(11, patchPrincipal8Detuned);
}
