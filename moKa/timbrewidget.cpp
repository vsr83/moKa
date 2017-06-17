
#include "timbrewidget.h"
#include <QDebug>
#include <math.h>

TimbreWidget::TimbreWidget(int _numHarmonics, QWidget *parent)
    : QWidget(parent), numHarmonics(_numHarmonics) {
    reset();
    setDetuneRange(0.00001, 0.5);

    hbox = new QHBoxLayout;
    setLayout(hbox);

    for (int harmonic = 0; harmonic < _numHarmonics; harmonic++) {
        QVBoxLayout *vbox = new QVBoxLayout;
        hbox->addLayout(vbox);

        QLabel *label = new QLabel(QString::number(harmonic+1));
        vbox->addWidget(label);
        vbox->setAlignment(label, Qt::AlignHCenter);

        QLabel *label2  = new QLabel(QString("0"));
        vbox->addWidget(label2);
        vbox->setAlignment(label2, Qt::AlignHCenter);

        QSlider *slider = new QSlider(Qt::Vertical);
        vbox->addWidget(slider);
        slider->setMinimum(0);
        slider->setMaximum(100);
        slider->setMinimumHeight(100);
        slider->setTickPosition(QSlider::TicksBothSides);
        vbox->setAlignment(slider, Qt::AlignHCenter);

        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));

        QDial *dial = new QDial;
        vbox->addWidget(dial);
        dial->setMinimum(1);
        dial->setMaximum(32);
        dial->setMaximumSize(30, 30);

        QDial *dial2 = new QDial;
        vbox->addWidget(dial2);
        dial2->setMinimum(-100);
        dial2->setMaximum(100);
        dial2->setMaximumSize(30, 30);

        vbox->setAlignment(dial,  Qt::AlignHCenter);
        vbox->setAlignment(dial2, Qt::AlignHCenter);

        connect(dial, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
        connect(dial2, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));

        coefficientLabels.push_back(label);
        detuneLabels.push_back(label2);
        amplitudeSliders.push_back(slider);
        coefficientDials.push_back(dial);
        detuneDials.push_back(dial2);
    }
    setSliderSteps(100);
    setSliderRange(0.0, 1.0);
    updateValues();
}

void
TimbreWidget::reset() {
    amplitudes   = QVector<double>(numHarmonics, 0);
    coefficients = QVector<double>(numHarmonics, 0);
    amplitudes[0] = 1.0;
}

void
TimbreWidget::updateValues() {
    for (int harm = 0; harm < numHarmonics; harm++) {
        qDebug() << harm << amplitudes[harm] << " "<<coefficients[harm];
        amplitudeSliders[harm]->blockSignals(true);
        amplitudeSliders[harm]->setValue(sliderDoubleUns(amplitudes[harm]));
        amplitudeSliders[harm]->blockSignals(false);

        coefficientDials[harm]->blockSignals(true);
        coefficientDials[harm]->setValue(round(coefficients[harm]));
        coefficientDials[harm]->blockSignals(false);

        double detuneCoeff = coefficients[harm] - round(coefficients[harm]);

        detuneDials[harm]->blockSignals(true);
        detuneDials[harm]->setValue(detuneDoubleInt(detuneCoeff));
        detuneDials[harm]->blockSignals(false);
    }
    updateLabels();
}

void
TimbreWidget::updateLabels() {
    for (int harm = 0; harm < numHarmonics; harm++) {
        double detuneCoeff = coefficients[harm] - round(coefficients[harm]);
        coefficientLabels[harm]->setText(QString::number(round(coefficients[harm])));
        detuneLabels[harm]->setText(QString::number(detuneDials[harm]->value()));
    }

}

void
TimbreWidget::valueChanged(int tmp) {
    amplitudes.clear();
    coefficients.clear();

    for (int harm = 0; harm < numHarmonics; harm++) {
        amplitudes.push_back(sliderUnsDouble(amplitudeSliders[harm]->value()));

        double coeff = coefficientDials[harm]->value()
                     + detuneIntDouble(detuneDials[harm]->value());

        coefficients.push_back(coeff);
    }
    emit settingsChanged(amplitudes, coefficients);
    updateLabels();
    Q_UNUSED(tmp);
}

void
TimbreWidget::setValues(QVector<double> &amplitudesIn,
                        QVector<double> &coefficientsIn) {
    qDebug() << "setValues " << amplitudesIn.size();
    Q_ASSERT(amplitudesIn.size() == coefficientsIn.size());

    if (amplitudesIn.size() < numHarmonics) {
        for (unsigned int indHarm = 0; indHarm < amplitudesIn.size(); indHarm++) {
            amplitudes[indHarm] = amplitudesIn[indHarm];
            coefficients[indHarm] = coefficientsIn[indHarm];
        }
        for (unsigned int indHarm = amplitudesIn.size(); indHarm < numHarmonics; indHarm++) {
            amplitudes[indHarm] = 0.0;
            coefficients[indHarm] = indHarm;
        }
    } else {
        amplitudes   = amplitudesIn;
        coefficients = coefficientsIn;
    }
    updateValues();
    qDebug() << " setValues";
}

void
TimbreWidget::getValues(QVector<double> &amplitudesOut,
                        QVector<double> &coefficientsOut) {
    amplitudesOut   = amplitudes;
    coefficientsOut = coefficients;
}

TimbreWidget::~TimbreWidget() {
}

void
TimbreWidget::setSliderSteps(unsigned int _sliderSteps) {
    sliderSteps = _sliderSteps;

    for (unsigned int indSlider = 0; indSlider < amplitudeSliders.size(); indSlider++) {
        amplitudeSliders[indSlider]->setRange(0, sliderSteps);
    }
}

void
TimbreWidget::setSliderRange(double _sliderMin,
                             double _sliderMax) {
    sliderMin = _sliderMin;
    sliderMax = _sliderMax;
}

unsigned int
TimbreWidget::sliderDoubleUns(double val) {
    if (val == 0.0) return 0;
    int step = (int)(sliderSteps *  (val - sliderMin) / (sliderMax - sliderMin));

    if (step < 0) {
        return 0;
    } else if (step >= sliderSteps) {
        return sliderSteps - 1;
    } else {
        return step;
    }
}

double
TimbreWidget::sliderUnsDouble(unsigned int step) {
    double val =  sliderMin + ((double)step) * (sliderMax - sliderMin) / sliderSteps;
    if (step == 0) return 0.0;
    if (val < sliderMin) {
        return sliderMin;
    } else if (val > sliderMax) {
        return sliderMax;
    }
    return val;
}

void
TimbreWidget::setCoefficients(QVector<double> &_coefficients) {
    coefficients = _coefficients;
    updateValues();
}

void
TimbreWidget::setSliderMode(bool linear) {

}

void
TimbreWidget::setAmplitudes(QVector<double> &_amplitudes) {
    amplitudes = _amplitudes;
    updateValues();
}

void
TimbreWidget::setDetuneEnabled(bool enabled) {

}

void
TimbreWidget::setDetuneRange(double _detuneMin,
                             double _detuneMax) {
    detuneMax = _detuneMax;
    detuneMin = _detuneMin;
}

int
TimbreWidget::detuneDoubleInt(double val) {
    double absValue = fabs(val);
    unsigned int nSteps = 100;

    if (absValue <= detuneMin) return 0;

    double a = (1.0/(double)(nSteps - 1)) * log10(detuneMin/detuneMax);
    unsigned int absStep = nSteps - (int)((1.0/a) * log10(absValue/detuneMax));

    if (absStep > nSteps) absStep = 100;

    qDebug() << "detuneDoubleInt " << val << " " << absStep;

    if (val > 0.0) {
        return absStep;
    } else {
        return -absStep;
    }
}

double
TimbreWidget::detuneIntDouble(int step) {
    unsigned int absStep = abs(step);
    unsigned int nSteps = 100;

    if (step == 0) return 0.0;
    double sign = 1.0;
    if (step < 0) sign = -1.0;

    double a = (1.0/(double)(nSteps - 1)) * log10(detuneMin/detuneMax);

    qDebug() << "detuneIntDouble " << step << " " << detuneMax * pow(10.0, a*(double)(nSteps - absStep));

    return sign * detuneMax * pow(10.0, a*(double)(nSteps - absStep));
}
