#include "form_specviewer1.h"
#include "ui_form_specviewer1.h"
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include "fmt/core.h"
#include <cmath>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QGuiApplication>

Form_SpecViewer1::Form_SpecViewer1(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form_SpecViewer1)
{
    ui->setupUi(this);
    ui->FormB->hide();
    ui->bandwidth_input->hide();
    ui->bandwidth_label->hide();
    ui->spectrumviewer->hide();
    setupController();
}

Form_SpecViewer1::~Form_SpecViewer1()
{
    delete ui;
}

void Form_SpecViewer1::dragEnterEvent(QDragEnterEvent* event)
{
    auto mimedata = event->mimeData();
    if (mimedata->hasUrls())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}


void Form_SpecViewer1::dragMoveEvent(QDragMoveEvent* event)
{
    auto mimedata = event->mimeData();
    if (mimedata->hasUrls())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}


void Form_SpecViewer1::dropEvent(QDropEvent* event)
{
    auto mimedata = event->mimeData();
    if (mimedata->hasUrls())
    {
        event->setDropAction(Qt::CopyAction);
        controller_accept_urls(mimedata->urls());
    }
}



void Form_SpecViewer1::setupController()
{
    QObject::connect(ui->confirm_button, &QPushButton::clicked,
                     this, &Form_SpecViewer1::controller_confirmedBtnPressed);


    QObject::connect(ui->startfreq_input, &QSpinBox::valueChanged,
                     this, &Form_SpecViewer1::controller_startfreq_changed);

    QObject::connect(ui->centerfreq_input, &QSpinBox::valueChanged,
                     this, &Form_SpecViewer1::controller_centerfreq_changed);

    QObject::connect(ui->stopfreq_input, &QSpinBox::valueChanged,
                     this, &Form_SpecViewer1::controller_stopfreq_changed);

    QObject::connect(ui->bandwidth_input, &QSpinBox::valueChanged,
                     this, &Form_SpecViewer1::controller_bandwidth_changed);

    QObject::connect(ui->samplingrate_input, &QSpinBox::valueChanged,
                     this, &Form_SpecViewer1::controller_samplingrate_changed);

    QObject::connect(ui->isReal_input, &QCheckBox::stateChanged,
                     this, &Form_SpecViewer1::controller_samplingrate_changed);

    QObject::connect(ui->nfft_input, &QSpinBox::valueChanged,
                     this, &Form_SpecViewer1::controller_nfft_changed);

    QObject::connect(ui->stride_input, &QSpinBox::valueChanged,
                     this, &Form_SpecViewer1::update_stft_size);

    QObject::connect(ui->starttime_input, &QDateTimeEdit::dateTimeChanged,
                     this, &Form_SpecViewer1::controller_starttime_changed);

    QObject::connect(ui->stoptime_input, &QDateTimeEdit::dateTimeChanged,
                     this, &Form_SpecViewer1::controller_stoptime_changed);

    QObject::connect(ui->nextfastlen_btn, &QPushButton::clicked,
                     this, &Form_SpecViewer1::controller_nextfastlen_pressed);

    QObject::connect(ui->prevfastlen_btn, &QPushButton::clicked,
                     this, &Form_SpecViewer1::controller_prevfastlen_pressed);

    QObject::connect(ui->freqresolution_input, &QDoubleSpinBox::valueChanged,
                     this, &Form_SpecViewer1::controller_freqres_changed);

    QObject::connect(ui->stride_fit_btn, &QPushButton::clicked,
                     this, &Form_SpecViewer1::stride_fit_pressed);

    QObject::connect(ui->stride_halfnfft, &QPushButton::clicked,
                     this, &Form_SpecViewer1::stride_halfnfft_pressed);

    QObject::connect(ui->dynamicrange_input, &QAbstractSlider::valueChanged,
                     this, &Form_SpecViewer1::controller_updatedynamicrange);

}

/* Starting Trigger */

void Form_SpecViewer1::controller_accept_urls(QList<QUrl> urls)
{
    std::sort(urls.begin(), urls.end());
    std::filesystem::path p{urls[0].toLocalFile().toStdString()};
    int filesize = std::filesystem::file_size(p);
    int header = filesize % 10000;
    filesize -= header;
    int samplingrate = filesize / 4;
    ui->header_input->setValue(header);
    ui->samplingrate_input->setValue(samplingrate);
    ui->startfreq_input->setValue(0);


    ui->urltable->setRowCount(urls.size()); // setRowCount must be done before setItem
    for (int i = 0; i < urls.size(); ++i)
    {
        auto filepath = urls[i].toLocalFile();
        m_filepaths.append(filepath);
        std::filesystem::path p{filepath.toStdString()};
        int filesize = std::filesystem::file_size(p);

        // TO CHECK: do I need to delete the QTableWidgetItem when this form is deleted?
        auto item0 = new QTableWidgetItem(QString::number(filesize));
        auto item1 = new QTableWidgetItem(filepath);

        ui->urltable->setItem(i,0, item0);
        ui->urltable->setItem(i,1, item1);

        m_totalbytes += filesize;
    }

    calculate_duration();

    ui->countlabel->setText(QString::fromStdString(fmt::format("Total Files: {}", urls.size())));
    ui->sizelabel->setText(QString::fromStdString(fmt::format("Total Size: {}", m_totalbytes)));
    ui->instruction_label->hide();
    ui->FormB->show();

    controller_freqres_changed();
    ui->stoptime_input->setDateTime(QDateTime::currentDateTime());

    stride_fit_pressed();

}

/* Time Freq Info */

void Form_SpecViewer1::calculate_duration()
{

    int nsamples = (m_totalbytes - m_filepaths.size()* (ui->header_input->value())) / 2;

    if (! ui->isReal_input->isChecked())
    {
        nsamples = nsamples / 2;
    }

    double dur = nsamples / ui->samplingrate_input->value();

    ui->duration->setText(QString::number(dur));
}

void Form_SpecViewer1::controller_startfreq_changed()
{

    int new_stopfreq = ui->startfreq_input->value() + ui->bandwidth_input->value();
    if (ui->stopfreq_input->value() != new_stopfreq){
        ui->stopfreq_input->setValue(new_stopfreq);
    }
    int new_centerfreq = ui->startfreq_input->value() + ui->bandwidth_input->value()/2;
    if (ui->centerfreq_input->value() != new_centerfreq){
        ui->centerfreq_input->setValue(new_centerfreq);
    }

}

void Form_SpecViewer1::controller_centerfreq_changed()
{

    int new_startfreq = ui->centerfreq_input->value() - ui->bandwidth_input->value()/2;
    if (ui->startfreq_input->value() != new_startfreq){
        ui->startfreq_input->setValue(new_startfreq);
    }
    int new_stopfreq = ui->centerfreq_input->value() + ui->bandwidth_input->value()/2;
    if (ui->stopfreq_input->value() != new_stopfreq) {
        ui->stopfreq_input->setValue(new_stopfreq);
    }
}

void Form_SpecViewer1::controller_stopfreq_changed()
{
    int new_bandwidth = ui->stopfreq_input->value() - ui->startfreq_input->value();
    if (ui->bandwidth_input->value() != new_bandwidth){
        ui->bandwidth_input->setValue(new_bandwidth);
    }
    int new_centerfreq = (ui->startfreq_input->value() + ui->stopfreq_input->value())/2;
    if (ui->centerfreq_input->value() != new_centerfreq){
        ui->centerfreq_input->setValue(new_centerfreq);
    }

}

void Form_SpecViewer1::controller_bandwidth_changed()
{
    int new_stopfreq = ui->startfreq_input->value() + ui->bandwidth_input->value();
    if (ui->stopfreq_input->value() != new_stopfreq){
        ui->stopfreq_input->setValue(new_stopfreq);
    }

    calculate_duration();
}

void Form_SpecViewer1::controller_samplingrate_changed(int arg)
{
    int new_bandwidth = ui->samplingrate_input->value();
    if (ui->isReal_input->isChecked()){
        new_bandwidth = new_bandwidth/2;
    }

    if (ui->bandwidth_input->value() != new_bandwidth){
        ui->bandwidth_input->setValue(new_bandwidth);
    }

    update_stft_size();
}

void Form_SpecViewer1::controller_starttime_changed(const QDateTime newDT)
{
    auto stoptime = newDT.addSecs(ui->duration->text().toDouble());
    if (stoptime != ui->stoptime_input->dateTime())
    {
        ui->stoptime_input->setDateTime(stoptime);
    }
}
void Form_SpecViewer1::controller_stoptime_changed(const QDateTime newDT)
{
    auto starttime = newDT.addSecs(-1*ui->duration->text().toDouble());
    if (starttime != ui->starttime_input->dateTime())
    {
        ui->starttime_input->setDateTime(starttime);
    }
}

/* STFT parameters */
void Form_SpecViewer1::controller_nextfastlen_pressed()
{
    ui->nfft_input->setValue(nextfastlen(ui->nfft_input->value()));
}


int Form_SpecViewer1::nextfastlen(int n)
{
    int bestfac=2*n;
    // fast transform for multiples of 2,3,5,7
    for (int f7=1; f7<bestfac; f7*=7)
        for (int f75=f7; f75<bestfac; f75*=5)
        {
            int x=f75;
            while (x<n) x*=2;
            for (;;)
            {
                if (x<n)
                    x*=3;
                else if (x>n)
                {
                    if (x<bestfac) bestfac=x;
                    if (x&1) break;
                    x>>=1;
                }
                else // x=n
                    return n;
            }
        }

    return bestfac;
}
int Form_SpecViewer1::prevfastlen(int n)
{
    int bestfound = 1;
    // fast transform for multiples of 2,3,5,7
    for (int f7 = 1; f7 <= n; f7 *= 7)
        for (int f75 = f7; f75 <= n; f75 *= 5)
        {
            int x = f75;
            while (x*2 <= n) x *= 2;
            if (x > bestfound) bestfound = x;
            while (true)
            {
                if (x * 3 <= n) x *= 3;
                else if (x % 2 == 0) x /= 2;
                else break;

                if (x > bestfound) bestfound = x;
            }
        }
    return bestfound;
}


void Form_SpecViewer1::controller_prevfastlen_pressed()
{
    ui->nfft_input->setValue(prevfastlen(ui->nfft_input->value()));
}
void Form_SpecViewer1::controller_nfft_changed()
{
    int bandwidth = ui->bandwidth_input->value();
    double freqres = (double) bandwidth / (double) ui->nfft_input->value();
    if (freqres!= ui->freqresolution_input->value())
    {
        ui->freqresolution_input->setValue(freqres);
    }

    update_stft_size();

}
void Form_SpecViewer1::update_stft_size()
{
    size_t nsamples = (m_totalbytes - m_filepaths.size()* ui->header_input->value()) / 2;
    if (!ui->isReal_input->isChecked())
    {
        nsamples = nsamples / 2;
    }
    size_t height = ui->isReal_input->isChecked() ? ui->nfft_input->value() /2 + 1 : ui->nfft_input->value();
    size_t width = (nsamples - ui->nfft_input->value()) / ui->stride_input->value() + 1;

    float OSR = ((double) (height * width)) / nsamples;

    ui->stftsize->setText(QString::fromStdString(fmt::format("Height: {0}   Width: {1}   OverSamplingRatio: {2}", height, width, OSR)));

    if (width %4)
    {
        size_t newwidth = width;
        size_t new_stride = ui->stride_input->value();

        while(newwidth %4)
        {
            new_stride += 1;
            newwidth = (nsamples - ui->nfft_input->value()) /new_stride + 1;
        }
        if (new_stride!=ui->stride_input->value() )
        {
            ui->stride_input->setValue(new_stride);
            update_stft_size();
        }
    }


}

void Form_SpecViewer1::controller_freqres_changed()
{
    int bandwidth = ui->bandwidth_input->value();
    double nfft = (double) bandwidth / (double) ui->freqresolution_input->value();
    int nfft_int = std::round(nfft);
    if (nfft_int!= ui->nfft_input->value())
    {
        ui->nfft_input->setValue(nfft_int);
        update_stft_size();
    }
}

void Form_SpecViewer1::stride_fit_pressed()
{
    auto screensize = QGuiApplication::primaryScreen()->size();
    int width = screensize.width();
    int nsamples = ui->duration->text().toDouble() * ui->samplingrate_input->value();
    int stride = (nsamples - ui->nfft_input->value()) / width;

    ui->stride_input->setValue(stride);
}


void Form_SpecViewer1::stride_halfnfft_pressed()
{
    ui->stride_input->setValue(ui->nfft_input->value()/2);
}


void Form_SpecViewer1::controller_updatedynamicrange(int a)
{
    ui->dynamicrange_value->setText(QString("%1dB").arg(a));
}

/* Confirm Button */

void Form_SpecViewer1::controller_confirmedBtnPressed()
{
    ui->spectrumviewer->displaydata(m_filepaths,
                                    ui->header_input->value(),
                                    ui->isReal_input->isChecked(),
                                    ui->nfft_input->value(),
                                    ui->stride_input->value(),
                                    ui->dynamicrange_input->value());

    ui->spectrumviewer->setFreqTimeRange(
        ui->startfreq_input->value(), // startF
        ui->stopfreq_input->value(), // stopF,
        ui->starttime_input->dateTime(), // startT,
        ui->stoptime_input->dateTime()); // stopT);

    ui->FormB->hide();
    ui->spectrumviewer->show();
}
