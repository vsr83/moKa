#include "midiconfiguration.h"
#include "RtMIDIInterface.h"

#include <QDebug>

MIDIConfiguration::MIDIConfiguration(QWidget *parent) : QWidget(parent) {
    vBoxLayout = new QVBoxLayout(this);
    MIDIListWidget = new QListWidget;
    vBoxLayout->addWidget(MIDIListWidget);
    setWindowTitle(tr("MIDI Input Sources"));

    MIDIListWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    updateList();
    hBoxLayout = new QHBoxLayout;
    vBoxLayout->addLayout(hBoxLayout);
    refreshButton = new QPushButton(tr("Refresh"));
    hBoxLayout->addWidget(refreshButton);

    connect(MIDIListWidget, SIGNAL(itemSelectionChanged()),
            this,           SLOT(itemSelectionChanged()));
    connect(refreshButton,  SIGNAL(clicked(bool)),
            this,           SLOT(refreshClicked(bool)));
}

MIDIConfiguration::~MIDIConfiguration() {

}

void
MIDIConfiguration::updateList() {
    RtMIDIInterface interface;

    MIDIListWidget->clear();

    for (unsigned int indPort = 0; indPort < interface.getNumberOfPorts(); indPort++) {
        QString portName = QString::fromStdString(interface.getPortName(indPort));

        MIDIListWidget->addItem(portName);
    }
}

void
MIDIConfiguration::createSelectionStatusSignal() {
    QVector<QString>      MIDIIFList;
    QVector<unsigned int> MIDIIFCurrentNumbers;
    QVector<bool>         MIDIIFSelectionStatus;

    for (unsigned int indRow = 0; indRow < (unsigned int)MIDIListWidget->count(); indRow++) {
        QListWidgetItem *item;

        item = MIDIListWidget->item(indRow);
        MIDIIFList.push_back(item->text());
        MIDIIFSelectionStatus.push_back(item->isSelected());
        MIDIIFCurrentNumbers.push_back(MIDIListWidget->row(item));
    }
    emit selectionStatus(MIDIIFList, MIDIIFCurrentNumbers, MIDIIFSelectionStatus);
}

void
MIDIConfiguration::itemSelectionChanged() {
    qDebug() << "Item Selection Changed";
    createSelectionStatusSignal();


    //QList <QListWidgetItem*> selectionList;
    //selectionList = MIDIListWidget->selectedItems();

    /*
    for (QList<QListWidgetItem *>::iterator it=selectionList.begin(); it != selectionList.end(); it++) {
        QListWidgetItem *item;

        item = *it;
        emit setActive(MIDIListWidget->row);
        qDebug() << MIDIListWidget->row(item) << item->text();
    }*/
}

void
MIDIConfiguration::refreshClicked(bool enabled) {
    updateList();;
    createSelectionStatusSignal();
}
