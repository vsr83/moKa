#ifndef ADSRPLOT_H
#define ADSRPLOT_H

#include <QWidget>
#include <QPixmap>
#include "envelope.h"

class ADSRPlot : public QWidget {
    Q_OBJECT
public:
    ADSRPlot(QWidget *parent = 0);
    ~ADSRPlot();
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
public slots:
    void setValues(Envelope &_envelope);
private:
    void refreshPixmap();
    Envelope envelope;

    QColor bgColor;
    QPixmap pixmap;
    int border;
};

#endif // ADSRPLOT_H
