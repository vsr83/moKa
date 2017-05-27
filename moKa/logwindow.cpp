#include "logwindow.h"
#include <QWidget>
#include <QFontDatabase>

LogWindow::LogWindow(QWidget *parent) : QTextEdit(parent) {
    setReadOnly(true);
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFont(fixedFont);
    setWindowTitle(tr("Log"));
    setMinimumSize(800,100);
}

LogWindow::~LogWindow() {

}

void
LogWindow::addLine(const QString &str) {
    append(str);
}
