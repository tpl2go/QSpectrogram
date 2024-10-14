#ifndef SPECTRUMVIEWER_H
#define SPECTRUMVIEWER_H

#include <QWidget>
#include <vector>
#include "fftw_allocator.h"
#include <QGraphicsScene>
#include <QDateTime>
#include <memory>
#include <immintrin.h>


namespace Ui {
class SpectrumViewer;
}

class SpectrumViewer : public QWidget
{
    Q_OBJECT

public:
    explicit SpectrumViewer(QWidget *parent = nullptr);
    ~SpectrumViewer();
    void setFreqTimeRange(int startF, int stopF, QDateTime startT, QDateTime stopT);
    void displaydata(QStringList filepaths, int headeroffset, bool isreal, int nfft, int stride, float dynamicrange);
    Ui::SpectrumViewer *ui;

// protected:
//     void resizeEvent(QResizeEvent *event);

private:

    QGraphicsScene *m_scene = nullptr;
    std::vector<uchar> m_bytemap;
    std::vector<uchar> m_bytemap_transpose;
    QImage m_img;
    QPixmap m_pix;

    std::vector<float> readdata(QStringList filepaths, int headeroffset);
    fftwf_vector makestft(std::vector<float>& data, bool isreal, int nfft, int stride);
    void quantizeSpectrogram(fftwf_vector& zxx, int ntime, int nfreq, float dynamicrange);
    void display(fftwf_vector& zxx,  int height, int width);

};

#endif // SPECTRUMVIEWER_H
