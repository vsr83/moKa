#ifndef MIDICONFIGURATION_H
#define MIDICONFIGURATION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVector>

class MIDIConfiguration : public QWidget {
    Q_OBJECT
public:
    MIDIConfiguration(QWidget *parent = 0);
    ~MIDIConfiguration();

    void updateList();
private:
    QVBoxLayout *vBoxLayout;
    QHBoxLayout *hBoxLayout;
    QListWidget *MIDIListWidget;
    QPushButton *refreshButton;

    void createSelectionStatusSignal();
signals:
    // In order for the ManagerQt to detect disconnected devices, the whole
    // array of devices and their selection status should be contained in
    // each signal. Since the numbering of the devices tends to change when
    // devices are disconnected, device name is used as a unique identifier:
    void selectionStatus(const QVector<QString>      MIDIIFList,
                         const QVector<unsigned int> MIDIIFCurrentNumbers,
                         const QVector<bool>         MIDIIFSelectionStatus);
private slots:
    void itemSelectionChanged();
    void refreshClicked(bool enabled);
};

#endif // MIDICONFIGURATION_H
