#include "mainwindow.h"
#include "managerqt.h"

#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    MDI = new QMdiArea();
    MDI->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    MDI->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(MDI);

    setWindowTitle("MöKä v.0.1");
    createMenus();

    Qt::WindowFlags flags;
    flags = Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowTitleHint;
    logWindow = new LogWindow();
    MDI->addSubWindow(logWindow, flags);

    managerQt = new ManagerQt();
    connect(managerQt, SIGNAL(debugText(const QString &)),
            logWindow, SLOT(addLine(const QString &)));

    emit managerQt->debugText(tr("Opening Audio Interface"));
    managerQt->openAudioInterface();
    emit managerQt->debugText(tr("Opening MIDI"));
    //managerQt->openMIDI(1);

    for (unsigned int chan = 0; chan < 5; chan++) {
        //if (chan == 0)
        kbWidget = new KBWidget(chan);
        //if (chan == 1)
        //kbWidget = new KBWidget(8);
        MDI->addSubWindow(kbWidget);
        connect(managerQt, SIGNAL(signalNoteOn(unsigned char,unsigned char,unsigned char)),
                kbWidget, SLOT(note_on_nosignal(unsigned char,unsigned char,unsigned char)));
        connect(managerQt, SIGNAL(signalNoteOff(unsigned char,unsigned char)),
                kbWidget, SLOT(note_off_nosignal(unsigned char,unsigned char)));

        connect(kbWidget, SIGNAL(sig_note_on(unsigned char,unsigned char,unsigned char)),
                managerQt,SLOT(noteOn(unsigned char,unsigned char,unsigned char)));
        connect(kbWidget, SIGNAL(sig_note_off(unsigned char,unsigned char)),
                managerQt,SLOT(noteOff(unsigned char,unsigned char)));
    }

    reverbWidget = new ReverbWidget();
    MDI->addSubWindow(reverbWidget, flags);
    connect(reverbWidget, SIGNAL(debugText(const QString &)),
            logWindow,    SLOT(addLine(const QString &)));
    connect(reverbWidget, SIGNAL(setReverb(bool,double,double)),
            managerQt,    SLOT(setReverb(bool,double,double)));
    connect(managerQt,    SIGNAL(initializeReverb(bool,double,double)),
            reverbWidget, SLOT(setValue(bool,double,double)));
    reverbWidget->show();

    //emit managerQt->initializeReverb(true, 0.75, 0.75);
    reverbWidget->setValue(true, 0.75, 0.75);

    midiConfiguration = new MIDIConfiguration();
    connect(midiConfiguration, SIGNAL(selectionStatus(QVector<QString>,QVector<uint>,QVector<bool>)),
            managerQt,         SLOT(MIDIIFSelectionStatus(QVector<QString>,QVector<uint>,QVector<bool>)));
    MDI->addSubWindow(midiConfiguration, flags);

    patchConfiguration = new PatchConfiguration(16);
    connect(patchConfiguration, SIGNAL(setPatch(uint,Patch)),
            managerQt,          SLOT(setPatch(uint,Patch)));
    patchConfiguration->configureDefaultPatches();
    MDI->addSubWindow(patchConfiguration, flags);
}

MainWindow::~MainWindow() {

}

void
MainWindow::closeEvent(QCloseEvent *event) {
    MDI->closeAllSubWindows();
}

void
MainWindow::updateWindowMenu() {
    windowMenu->clear();
}

void
MainWindow::createMenus() {
    newAction = new QAction(tr("&New"), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new patch"));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit möKä"));

    aboutAction = new QAction(tr("About möKä"), this);
    aboutQtAction = new QAction(tr("About &Qt"), this);

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    menuBar()->addSeparator();

    QMenuBar *helpMenuBar = new QMenuBar();
    menuBar()->setCornerWidget(helpMenuBar);
    helpMenu = new QMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
    helpMenuBar->addMenu(helpMenu);

    connect(exitAction, SIGNAL(triggered(bool)),
            this, SLOT(quit()));    
}

void
MainWindow::quit() {
    QApplication::quit();
}
