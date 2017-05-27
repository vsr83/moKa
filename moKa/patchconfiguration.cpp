#include "patchconfiguration.h"

PatchConfiguration::PatchConfiguration(QWidget *parent) : QWidget(parent) {
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
    defaultEnv.sustainAmplitude = 0.5;

    Filter     filterLP(Filter::FILTER_LOWPASS,
                        Filter::WINDOW_RECT,
                        256,
                        44100,
                        4000.0);
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

    std::vector<double> detuneAmpl = {0.5, 0.5};
    std::vector<double> detuneCoeff = {1.0, 1.01};

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


    emit setChannelPatch(0, patchQuintadena8);
    emit setChannelPatch(1, patchSubbas16);
    emit setChannelPatch(2, patchPrincipal8);
    emit setChannelPatch(3, patchQuintadena8Detuned);
    emit setChannelPatch(4, patchSubbas16Detuned);
    emit setChannelPatch(5, patchPrincipal8Detuned);
    emit setChannelPatch(6, patchTrumpet);
    emit setChannelPatch(7, patchFlute);
}
