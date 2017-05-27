#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMenu>
#include <QMenuBar>
#include <QHBoxLayout>

#include "managerqt.h"
#include "logwindow.h"
#include "kbWidget.h"
#include "reverbwidget.h"
#include "midiconfiguration.h"
#include "patchconfiguration.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *);
public slots:
    void quit();
private:
    void createMenus();
    void updateWindowMenu();

    ManagerQt *managerQt;

    QMdiArea * MDI;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;

    QAction *newAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *aboutQtAction;

    MIDIConfiguration *midiConfiguration;

    LogWindow          *logWindow;
    KBWidget           *kbWidget;
    ReverbWidget       *reverbWidget;
    PatchConfiguration *patchConfiguration;
};

#endif // MAINWINDOW_H
