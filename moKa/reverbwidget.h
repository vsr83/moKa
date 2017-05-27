#ifndef REVERBWIDGET_H
#define REVERBWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QDial>


class ReverbWidget : public QWidget {
    Q_OBJECT
public:
    ReverbWidget(QWidget *parent = 0);
    ~ReverbWidget();

    QLabel *wetLabel;
    QLabel *roomSizeLabel;

    QLabel *wetValueLabel;
    QLabel *roomSizeValueLabel;

    QDial *wetDial;
    QDial *roomSizeDial;

    QCheckBox   *activeBox;
    QGridLayout *gridLayout;
public slots:
    void setValue(bool   _enabled,
                  double _wetValue,
                  double _roomSizeValue);

    void updateEnabled(bool _enabled);
    void updateWet(int wet);
    void updateRoomSize(int roomSize);
signals:
    void setReverb(bool   enabled,
                   double wetValue,
                   double roomSizeValue);
    void debugText(const QString &text);
private:
    bool enabled;

    double wetValue;
    double roomSizeValue;

    double wetValueMin;
    double wetValueMax;
    double roomSizeValueMin;
    double roomSizeValueMax;
};

#endif // REVERBWIDGET_H
