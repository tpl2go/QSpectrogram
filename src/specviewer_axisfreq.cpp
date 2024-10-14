#include "specviewer_axisfreq.h"
#include "ui_specviewer_axisfreq.h"
#include <QPainter>
SpecViewer_AxisFreq::SpecViewer_AxisFreq(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SpecViewer_AxisFreq)
{
    ui->setupUi(this);
}

SpecViewer_AxisFreq::~SpecViewer_AxisFreq()
{
    delete ui;
}

void SpecViewer_AxisFreq::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setFont(QFont("Arial", 7));
    auto pen = QPen(Qt::black);
    pen.setWidth(0);
    painter.setPen(pen);
    painter.drawLine(
        width() -1, // startX
        0, // startY
        width() -1,  // stopX
        height() // stopY
        );

    painter.drawLine(
        0, // startX
        1, // startY
        width() -1,  // stopX
        1 // stopY
        );

    painter.drawLine(
        0, // startX
        height()-1, // startY
        width() -1,  // stopX
        height()-1 // stopY
        );

    painter.rotate(-90);

/*
^ x
|
|
-----> y
*/

    auto viewRect = parentGraphicsView->mapToScene(parentGraphicsView->viewport()->geometry()).boundingRect().toRect();
    auto sceneRect = parentGraphicsView->scene()->sceneRect();

    int local_startF = (viewRect.top() - sceneRect.top()) / sceneRect.height() * (global_stopF - global_startF) + global_startF;
    int local_stopF = (viewRect.bottom() - sceneRect.top()) / sceneRect.height() * (global_stopF - global_startF) + global_startF;

    int xline = 7;
    auto startFstr = QString::number(local_startF);
    // painter.drawText(-5*startFstr.size()-3 ,xline,startFstr);
    painter.drawText(-height() + 3 ,xline,startFstr);

    auto stopFstr = QString::number(local_stopF);
    // painter.drawText(-height() + 3,xline, stopFstr);
    painter.drawText(-5*startFstr.size()-3,xline, stopFstr);

    auto centerFstr = QString::number((local_startF + local_stopF)/2);
    painter.drawText(-height()/ 2 -5*centerFstr.size()/2,xline,centerFstr);


}

void SpecViewer_AxisFreq::resizeEvent(QResizeEvent * event)
{
    update();
}

void SpecViewer_AxisFreq::triggerUpdate(int a)
{
    update();
}
