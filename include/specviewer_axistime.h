#ifndef SPECVIEWER_AXISTIME_H
#define SPECVIEWER_AXISTIME_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QDateTime>
#include <QGraphicsView>

namespace Ui {
class SpecViewer_AxisTime;
}

class SpecViewer_AxisTime : public QWidget
{
    Q_OBJECT

public:
    explicit SpecViewer_AxisTime(QWidget *parent = nullptr);
    ~SpecViewer_AxisTime();
    QGraphicsView* parentGraphicsView;
    QDateTime global_startT, global_stopT;
public slots:
    void triggerUpdate(int a);

protected:
    void paintEvent(QPaintEvent * event) override;
    void resizeEvent(QResizeEvent  * event) override;


private:
    Ui::SpecViewer_AxisTime *ui;
};

#endif // SPECVIEWER_AXISTIME_H
