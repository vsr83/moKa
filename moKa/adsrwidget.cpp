
#include "adsrwidget.h"
#include <QLabel>

ADSRWidget::ADSRWidget(QWidget *parent) : QWidget(parent) {
    envelope.attackTime = 100;
    envelope.decayTime = 400;
    envelope.releaseTime = 100;

    //envelope. = 0;
    envelope.peakAmplitude = 1;
    envelope.sustainAmplitude = 0.8;

    QLabel *attackLabel = new QLabel(tr("Attack:"));
    attackSB = new QSpinBox();
    attackSB->setRange(0, 1000);
    attackSB->setSuffix(tr(" ms"));
    attackSB->setValue(envelope.attackTime);

    QLabel *decayLabel = new QLabel(tr("Decay: "));
    decaySB = new QSpinBox();
    decaySB->setRange(0, 1000);
    decaySB->setSuffix(tr(" ms"));
    decaySB->setValue(envelope.decayTime);

    QLabel *releaseLabel = new QLabel(tr("Release: "));
    releaseSB = new QSpinBox();
    releaseSB->setRange(0, 1000);
    releaseSB->setSuffix(tr(" ms"));
    releaseSB->setValue(envelope.releaseTime);

    QLabel *initialLabel = new QLabel(tr("  Initial: "));
    initialSB = new QDoubleSpinBox();
    initialSB->setValue(0.0);
    initialSB->setRange(0, 1);
    initialSB->setSingleStep(0.01);

    QLabel *peakLabel = new QLabel(tr("  Peak: "));
    peakSB = new QDoubleSpinBox();
    peakSB->setValue(envelope.peakAmplitude);
    peakSB->setRange(0, 1);
    peakSB->setSingleStep(0.01);

    QLabel *sustainLabel = new QLabel(tr("  Sustain: "));
    sustainSB = new QDoubleSpinBox();
    sustainSB->setValue(envelope.sustainAmplitude);
    sustainSB->setRange(0, 1);
    sustainSB->setSingleStep(0.01);

    gridLayout = new QGridLayout;
    setLayout(gridLayout);

    attackModeB = new QComboBox;
    attackModeB->addItem(tr("Exponential"));
    attackModeB->addItem(tr("Linear"));

    decayModeB = new QComboBox;
    decayModeB->addItem(tr("Exponential"));
    decayModeB->addItem(tr("Linear"));

    releaseModeB = new QComboBox;
    releaseModeB->addItem(tr("Exponential"));
    releaseModeB->addItem(tr("Linear"));


    plot = new ADSRPlot;

    gridLayout->addWidget(attackLabel,  0, 0);
    gridLayout->addWidget(attackSB,     0, 1);
    gridLayout->addWidget(decayLabel,   1, 0);
    gridLayout->addWidget(decaySB,      1, 1);
    gridLayout->addWidget(releaseLabel, 2, 0);
    gridLayout->addWidget(releaseSB,    2, 1);

    gridLayout->addWidget(attackModeB,  0, 2);
    gridLayout->addWidget(decayModeB,   1, 2);
    gridLayout->addWidget(releaseModeB, 2, 2);

    gridLayout->addWidget(initialLabel,  0, 3);
    gridLayout->addWidget(initialSB,     0, 4);
    gridLayout->addWidget(peakLabel,     1, 3);
    gridLayout->addWidget(peakSB,        1, 4);
    gridLayout->addWidget(sustainLabel,  2, 3);
    gridLayout->addWidget(sustainSB,     2, 4);

    gridLayout->addWidget(plot, 0, 5, 4, 1);
    gridLayout->setColumnMinimumWidth(5, 150);

    connect(attackSB,    SIGNAL(valueChanged(int)),    this, SLOT(valueChangedInt(int)));
    connect(decaySB,     SIGNAL(valueChanged(int)),    this, SLOT(valueChangedInt(int)));
    connect(releaseSB,   SIGNAL(valueChanged(int)),    this, SLOT(valueChangedInt(int)));
    connect(initialSB,   SIGNAL(valueChanged(double)), this, SLOT(valueChangedDouble(double)));
    connect(peakSB,      SIGNAL(valueChanged(double)), this, SLOT(valueChangedDouble(double)));
    connect(sustainSB,   SIGNAL(valueChanged(double)), this, SLOT(valueChangedDouble(double)));
    connect(attackModeB, SIGNAL(activated(int)),       this, SLOT(setAttackMode(int)));
    connect(decayModeB,  SIGNAL(activated(int)),       this, SLOT(setDecayMode(int)));
    connect(releaseModeB,SIGNAL(activated(int)),       this, SLOT(setReleaseMode(int)));

    initialSB->setEnabled(false);

    plot->setValues(envelope);
}

ADSRWidget::~ADSRWidget() {
}

void
ADSRWidget::valueChangedInt(int val) {
    envelope.attackTime  = 0.001*(double)attackSB->value();
    envelope.decayTime   = 0.001*(double)decaySB->value();
    envelope.releaseTime = 0.001*(double)releaseSB->value();

    //envelope.initialAmpl = (qreal)initialSB->value();
    envelope.peakAmplitude    = (qreal)peakSB->value();
    envelope.sustainAmplitude = (qreal)sustainSB->value();

    plot->setValues(envelope);
    emit envelopeChanged(envelope);
    Q_UNUSED(val);
}

void
ADSRWidget::importEnvelope(Envelope &env) {
    envelope.attackTime  = env.attackTime;
    envelope.decayTime   = env.decayTime;
    //envelope.initialAmplitude = env.initialAmpl;
    envelope.peakAmplitude    = env.peakAmplitude;
    envelope.releaseTime = env.releaseTime;
    envelope.sustainAmplitude = env.sustainAmplitude;

    attackSB->setValue((int)(1000.0*envelope.attackTime));
    decaySB->setValue((int)(1000.0*envelope.decayTime));
    releaseSB->setValue((int)(1000.0*envelope.releaseTime));
    //initialSB->setValue(env.initialAmpl);
    peakSB->setValue(env.peakAmplitude);
    sustainSB->setValue(env.sustainAmplitude);
}

void
ADSRWidget::valueChangedDouble(double val) {
    valueChangedInt(0);
    Q_UNUSED(val);
}

void
ADSRWidget::setAttackMode(int mode) {
    envelope.setMode(mode,
                     Envelope::MODE_INVALID,
                     Envelope::MODE_INVALID);
}

void
ADSRWidget::setDecayMode(int mode) {
    envelope.setMode(Envelope::MODE_INVALID,
                     mode,
                     Envelope::MODE_INVALID);
}

void
ADSRWidget::setReleaseMode(int mode) {
    envelope.setMode(Envelope::MODE_INVALID,
                     Envelope::MODE_INVALID,
                     mode);
}
