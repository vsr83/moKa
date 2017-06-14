#ifndef ADSRWIDGET_H
#define ADSRWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpinBox>
#include <QComboBox>

#include "adsrplot.h"
#include "envelope.h"

class ADSRWidget : public QWidget {
    Q_OBJECT
public:
    ADSRWidget(QWidget *parent = 0);
    ~ADSRWidget();
    void importEnvelope(Envelope &env);
signals:
    void envelopeChanged(Envelope &_envelope);
private slots:
    void valueChangedInt(int val);
    void valueChangedDouble(double val);

    void setAttackMode(int mode);
    void setDecayMode(int mode);
    void setReleaseMode(int mode);
private:
    QSpinBox       *attackSB, *decaySB, *releaseSB;
    QDoubleSpinBox *initialSB, *peakSB, *sustainSB;
    QComboBox      *attackModeB, *decayModeB, *releaseModeB;

    QGridLayout *gridLayout;
    ADSRPlot *plot;

    Envelope envelope;
};

#endif // ADSRWIDGET_H
