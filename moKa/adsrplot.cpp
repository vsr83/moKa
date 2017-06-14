
#include "adsrplot.h"

#include <QPen>
#include <QPainter>
#include <QStylePainter>

ADSRPlot::ADSRPlot(QWidget *parent) : QWidget(parent) {
    border = 15;
    bgColor = palette().color(QPalette::Window);
}

ADSRPlot::~ADSRPlot() {
}

void
ADSRPlot::setValues(Envelope &_envelope) {
    envelope = _envelope;
    refreshPixmap();
}

void
ADSRPlot::refreshPixmap() {
    pixmap = QPixmap(size());
    pixmap.fill(bgColor);

    QPainter painter(&pixmap);

    QPen penWave, penGrid, penBorder;

    penWave.setStyle(Qt::SolidLine);
    penWave.setWidth(2);
    penWave.setColor(QColor(0, 0, 0));

    penGrid.setStyle(Qt::DashLine);
    penGrid.setWidth(1);
    penGrid.setColor(QColor(150, 150, 150));

    penBorder.setStyle(Qt::SolidLine);
    penBorder.setWidth(1);
    penBorder.setColor(QColor(0, 0, 0));

    unsigned int ww, wh;
    ww = width()  - 2*border;
    wh = height() - 2*border;

    qreal total = 1.5*(qreal)(envelope.attackTime
                            + envelope.decayTime
                            + envelope.releaseTime);
    unsigned int attackx = border
                         + (unsigned int)((qreal)ww * (qreal)envelope.attackTime / total),
                 decayx  = attackx
                         + (unsigned int)((qreal)ww * (qreal)envelope.decayTime / total),
                 releasex= border
                         + (unsigned int)((qreal)ww * (1-(qreal)envelope.releaseTime / total));


    unsigned int initialy = height() - border
                          - (unsigned int)((qreal)wh * 0.0),
                 peaky    = height() - border
                          - (unsigned int)((qreal)wh * envelope.peakAmplitude),
                 sustainy = height() - border
                          - (unsigned int)((qreal)wh * envelope.sustainAmplitude);

    painter.setPen(penGrid);
    painter.drawLine(border, initialy, ww+border, initialy);
    painter.drawLine(border, sustainy, ww+border, sustainy);
    painter.drawLine(border, peaky, ww+border, peaky);
    painter.drawLine(attackx,  border/2, attackx,  wh + border);
    painter.drawLine(decayx,   border/2, decayx,   wh + border);
    painter.drawLine(releasex, border/2, releasex, wh + border);

    painter.setPen(penBorder);
    painter.drawText(border/4, initialy+5, tr("I"));
    painter.drawText(border/4, sustainy+5, tr("S"));
    painter.drawText(border/4, peaky+5,    tr("P"));
    painter.drawText(attackx/2,  wh+border*2, tr("A"));
    painter.drawText((attackx + decayx)/2,   wh+border*2, tr("D"));
    painter.drawText((ww + border + releasex)/2, wh+border*2, tr("R"));

    painter.drawLine(border, height() - border, width()-border, height() - border);
    painter.drawLine(border, border, border, height() - border);

    painter.setPen(penWave);
    painter.drawLine(border, initialy, attackx, peaky);
    painter.drawLine(attackx, peaky, decayx, sustainy);
    painter.drawLine(decayx, sustainy, releasex, sustainy);
    painter.drawLine(releasex, sustainy, border+ww, border+wh);

    update();
}

void
ADSRPlot::paintEvent(QPaintEvent *event) {
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
    Q_UNUSED(event);
}

void
ADSRPlot::resizeEvent(QResizeEvent *event) {
    refreshPixmap();
    Q_UNUSED(event);
}
