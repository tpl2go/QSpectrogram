#include "specviewer_axistime.h"
#include "ui_specviewer_axistime.h"
#include <iostream>
SpecViewer_AxisTime::SpecViewer_AxisTime(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SpecViewer_AxisTime)
{
    ui->setupUi(this);
}

SpecViewer_AxisTime::~SpecViewer_AxisTime()
{
    delete ui;
}


void SpecViewer_AxisTime::paintEvent(QPaintEvent * event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setFont(QFont("Arial", 7));
    auto pen = QPen(Qt::black);
    pen.setWidth(0);
    painter.setPen(pen);
    painter.drawLine(
        0, // startX
        1, // startY
        width(),  // stopX
        1 // stopY
        );

    painter.drawLine(
        1, // startX
        0, // startY
        1,  // stopX
        height() // stopY
        );

    painter.drawLine(
        width()-1, // startX
        0, // startY
        width()-1,  // stopX
        height() // stopY
        );

    auto viewRect = parentGraphicsView->mapToScene(parentGraphicsView->viewport()->geometry()).boundingRect().toRect();
    auto sceneRect = parentGraphicsView->scene()->sceneRect();

    auto duration = global_startT.secsTo(global_stopT);
    auto mid = (viewRect.left() + viewRect.right())/2;

    auto global_left = sceneRect.left();
    auto local_startT = global_startT.addSecs((viewRect.left() - global_left) * ((double) duration)/ sceneRect.width()) ;
    auto local_stopF = global_startT.addSecs((viewRect.right() - global_left) * ((double) duration)/ sceneRect.width()) ;
    auto local_centerF = global_startT.addSecs((mid - global_left) * ((double) duration)/ sceneRect.width()) ;

    int yline = 10;
    auto startTstr = local_startT.toString("HH:mm:ss");
    auto stopTstr = local_stopF.toString("HH:mm:ss");
    auto centerTstr = local_centerF.toString("HH:mm:ss");

    painter.drawText(3,yline,startTstr);
    painter.drawText(width() -5*stopTstr.size() ,yline,stopTstr);
    painter.drawText(width()/ 2 -5*centerTstr.size()/2,yline,centerTstr);
}

void SpecViewer_AxisTime::resizeEvent(QResizeEvent *event)
{
    update();
}

void SpecViewer_AxisTime::triggerUpdate(int a)
{
    update();
}
