#ifndef TIMBREWIDGET_H
#define TIMBREWIDGET_H

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVector>
#include <QSlider>
#include <QDial>
#include <QLabel>

class TimbreWidget : public QWidget {
    Q_OBJECT

public:
    TimbreWidget(int _numHarmonics, QWidget *parent = 0);
    ~TimbreWidget();

    void getValues(QVector<double> &amplitudesOut,
                   QVector<double> &coefficientsOut);
    void setValues(QVector<double> &amplitudesIn,
                   QVector<double> &coefficientsIn);
public slots:
    void reset();
    void updateValues();
    void updateLabels();

    void setDetuneRange  (double _detuneMin,
                          double _detuneMax);
    void setSliderMode   (bool linear);
    void setSliderRange  (double _sliderMin,
                          double _sliderMax);
    void setSliderSteps  (unsigned int _sliderSteps);
    void setDetuneEnabled(bool enabled);
    void setAmplitudes   (QVector<double> &_amplitudes);
    void setCoefficients (QVector<double> &_coefficients);
private slots:
    void valueChanged(int tmp);
signals:
    void settingsChanged(QVector<double> &amplitudes,
                         QVector<double> &coefficients);
private:
    unsigned int sliderDoubleUns(double val);
    double       sliderUnsDouble(unsigned int step);

    int     detuneDoubleInt(double step);
    double  detuneIntDouble(int step);

    int numHarmonics;
    QHBoxLayout *hbox;

    QVector<QSlider *> amplitudeSliders;
    QVector<QDial *>   coefficientDials;
    QVector<QDial *>   detuneDials;
    QVector<QLabel *>  coefficientLabels;
    QVector<QLabel *>  detuneLabels;

    QVector<double> amplitudes;
    QVector<double> coefficients;

    double sliderMin, sliderMax;
    unsigned int sliderSteps;

    double detuneMin;
    double detuneMax;
};

#endif // TIMBREWIDGET_H
