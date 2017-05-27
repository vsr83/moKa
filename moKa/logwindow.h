#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QTextEdit>
#include <QWidget>
#include <QString>

class LogWindow : public QTextEdit {
    Q_OBJECT
public:
    LogWindow(QWidget *parent = 0);
    ~LogWindow();

public slots:
    void addLine(const QString &str);
};

#endif // LOGWINDOW_H
