#ifndef TIMBREWIDGET_H
#define TIMBREWIDGET_H

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVector>
#include <QSlider>
#include <QDial>
#include <QLabel>
#include <QSignalMapper>

class TimbreWidget : public QWidget {
    Q_OBJECT

public:
    TimbreWidget(int _numHarmonics, QWidget *parent = 0);
    ~TimbreWidget();

    void getValues(QVector<int> &amplitudesOut, QVector<int> &phasesOut);
    void setValues(QVector<int> &amplitudesIn, QVector<int> &phasesIn);
public slots:
    void reset();
    void updateValues();
private slots:
    void valueChanged(int tmp);
signals:
    void settingsChanged(QVector<int> &amplitudes, QVector<int> &phases);
private:
    int numHarmonics;
    QHBoxLayout *hbox;

    QVector<QSlider *> sliders;
    QVector<QDial *> dials;
    QVector<int> amplitudes;
    QVector<int> phases;

    QSignalMapper *signalMapper;
};

#endif // TIMBREWIDGET_H
