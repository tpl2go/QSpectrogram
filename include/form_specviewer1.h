#ifndef FORM_SPECVIEWER1_H
#define FORM_SPECVIEWER1_H

#include <QWidget>
#include <QUrl>
#include <QList>
#include <QDateTime>

namespace Ui {
class Form_SpecViewer1;
}

class Form_SpecViewer1 : public QWidget
{
    Q_OBJECT

public:
    explicit Form_SpecViewer1(QWidget *parent = nullptr);
    ~Form_SpecViewer1();
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent *event);
    QStringList m_filepaths;
    int m_totalbytes = 0;
    Ui::Form_SpecViewer1 *ui;

public slots:

    // Starting trigger
    void controller_accept_urls(QList<QUrl> urls);

    // Freq Info
    void controller_startfreq_changed();
    void controller_centerfreq_changed();
    void controller_stopfreq_changed();
    void controller_bandwidth_changed();
    void controller_samplingrate_changed(int arg);
    void calculate_duration();

    // Time Info
    void controller_starttime_changed(const QDateTime newDT);
    void controller_stoptime_changed(const QDateTime newDT);

    // STFT parameters
    void controller_nextfastlen_pressed();
    void controller_prevfastlen_pressed();
    void controller_nfft_changed();
    void controller_freqres_changed();
    void stride_fit_pressed();
    void update_stft_size();
    void controller_updatedynamicrange(int a);

    // Confirm Button
    void controller_confirmedBtnPressed();

private:

    void setupController();

    int nextfastlen(int n);
    int prevfastlen(int n);

};

#endif // FORM_SPECVIEWER1_H
