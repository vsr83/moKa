#include "reverbwidget.h"

#include <QTextStream>

ReverbWidget::ReverbWidget(QWidget *parent) : QWidget(parent) {
    gridLayout = new QGridLayout;
    setLayout(gridLayout);

    activeBox = new QCheckBox(tr("Active"));
    activeBox->setChecked(true);
    wetLabel = new QLabel(tr("Wet :"));
    roomSizeLabel = new QLabel(tr("Room Size :"));

    gridLayout->addWidget(activeBox, 0, 0, 1, 1);
    wetDial = new QDial;
    roomSizeDial = new QDial;

    wetDial->setMaximumSize(75, 75);
    roomSizeDial->setMaximumSize(75, 75);

    wetValueMin = 0;
    wetValueMax = 100;
    roomSizeValueMin = 0;
    roomSizeValueMax = 100;

    wetDial->setRange(0, 100);
    roomSizeDial->setRange(0, 100);
    wetDial->setTracking(true);
    roomSizeDial->setTracking(true);

    gridLayout->addWidget(wetDial,       0, 1, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(roomSizeDial,  0, 2, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(wetLabel,      1, 1, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(roomSizeLabel, 1, 2, 1, 1, Qt::AlignHCenter);

    connect(wetDial,      SIGNAL(valueChanged(int)), this, SLOT(updateWet(int)));
    connect(roomSizeDial, SIGNAL(valueChanged(int)), this, SLOT(updateRoomSize(int)));
    connect(activeBox,    SIGNAL(clicked(bool)) ,    this, SLOT(updateEnabled(bool)));

    setWindowTitle("FreeVerb");
}

ReverbWidget::~ReverbWidget() {

}

void
ReverbWidget::updateWet(int wet) {
    wetValue = ((double)wet) / 100.0;

    QString debugStr;
    QTextStream(&debugStr) << "REVERB - Set Parameters : "
                           << "enabled = " << enabled
                           << ", wet = " << wetValue
                           << ", roomSize = "  << roomSizeValue;
    emit debugText(debugStr);
    emit setReverb(enabled, wetValue, roomSizeValue);
}

void
ReverbWidget::updateRoomSize(int roomSize) {
    roomSizeValue = ((double)roomSize) / 100.0;
    QString debugStr;
    QTextStream(&debugStr) << "REVERB - Set Parameters : "
                           << "enabled = " << enabled
                           << ", wet = " << wetValue
                           << ", roomSize = "  << roomSizeValue;
    emit debugText(debugStr);
    emit setReverb(enabled, wetValue, roomSizeValue);
}

void
ReverbWidget::updateEnabled(bool _enabled) {
    enabled = _enabled;
    QString debugStr;
    QTextStream(&debugStr) << "REVERB - Set Parameters : "
                           << "enabled = " << enabled
                           << ", wet = " << wetValue
                           << ", roomSize = "  << roomSizeValue;
    emit debugText(debugStr);
    emit setReverb(enabled, wetValue, roomSizeValue);
}

void
ReverbWidget::setValue(bool _enabled,
                       double _wetValue,
                       double _roomSizeValue) {
    enabled = _enabled;
    wetValue = _wetValue;
    roomSizeValue = _roomSizeValue;

    int wetValueInt = (int)((wetValue - wetValueMin) / (wetValueMax - wetValueMin));
    int roomSizeValueInt = (int)((roomSizeValue - roomSizeValueMin)
                         / (roomSizeValueMax - roomSizeValueMin));

    QString debugStr;
    QTextStream(&debugStr) << "REVERB - Set Parameters : "
                           << "enabled = " << enabled
                           << ", wet = " << wetValue
                           << ", roomSize = "  << roomSizeValue;
    emit debugText(debugStr);
    wetDial->setValue(wetValueInt);
    roomSizeDial->setValue(roomSizeValueInt);

    //wetDial->setSliderPosition(50);
}
