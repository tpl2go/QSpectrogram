#include "form_specviewer2.h"
#include "ui_form_specviewer2.h"
#include "ui_form_specviewer1.h"
#include "ui_spectrumviewer.h"
#include <QAbstractSlider>

Form_SpecViewer2::Form_SpecViewer2(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form_SpecViewer2)
{
    ui->setupUi(this);

    // connect individual sliders' range changes to synch slider
    QObject::connect(ui->panel1->ui->spectrumviewer->ui->graphicsView->verticalScrollBar(),
                        &QAbstractSlider::rangeChanged,
        this->ui->verticalScrollBar, &QAbstractSlider::setRange);

    QObject::connect(ui->panel2->ui->spectrumviewer->ui->graphicsView->verticalScrollBar(),
                     &QAbstractSlider::rangeChanged,
                     this->ui->verticalScrollBar, &QAbstractSlider::setRange);

    // connect synch slider value change to individual sliders
    QObject::connect(this->ui->verticalScrollBar,
                     &QAbstractSlider::valueChanged,
                     ui->panel1->ui->spectrumviewer->ui->graphicsView->verticalScrollBar(), &QAbstractSlider::setValue);

    QObject::connect(this->ui->verticalScrollBar,
                     &QAbstractSlider::valueChanged,
                     ui->panel2->ui->spectrumviewer->ui->graphicsView->verticalScrollBar(),  &QAbstractSlider::setValue);
}

Form_SpecViewer2::~Form_SpecViewer2()
{
    delete ui;
}
