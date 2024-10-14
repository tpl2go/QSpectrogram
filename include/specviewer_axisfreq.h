#ifndef SPECVIEWER_AXISFREQ_H
#define SPECVIEWER_AXISFREQ_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsView>

namespace Ui {
class SpecViewer_AxisFreq;
}

class SpecViewer_AxisFreq : public QWidget
{
    Q_OBJECT

public:
    explicit SpecViewer_AxisFreq(QWidget *parent = nullptr);
    ~SpecViewer_AxisFreq();
    QGraphicsView* parentGraphicsView;
    int global_startF, global_stopF;
public slots:
    void triggerUpdate(int a);
protected:
    void paintEvent(QPaintEvent * event) override;
    void resizeEvent(QResizeEvent  * event) override;

private:
    Ui::SpecViewer_AxisFreq *ui;
};

#endif // SPECVIEWER_AXISFREQ_H
