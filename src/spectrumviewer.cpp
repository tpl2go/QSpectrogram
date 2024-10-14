#include "spectrumviewer.h"
#include "ui_spectrumviewer.h"
#include <cfloat>
#include <iostream>
#include <fstream>
#include "fftw3.h"
#include "fftw_allocator.h"
#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <iostream>
#include <cmath>
#include "fmt/core.h"
#include <QScrollBar>
#include <complex>
#include "specviewer_axisfreq.h"

template<typename T>
void transpose(const std::vector<T>& input, std::vector<T>& output, size_t W, size_t H) {
    // Resize the output to hold the transposed matrix
    output.resize(input.size());
    #pragma omp parallel for
    for (int i = 0; i < H; ++i) {
        #pragma omp simd
        for (int j = 0; j < W; ++j) {
            output[j * H + i] = input[i * W + j];
        }
    }
}

template<typename T>
void fftshift(const T* input, T* output, size_t W, size_t H) {

    /*
    fftshift([1,2,3,4,5]) = [4,5,1,2,3]
    fftshift([1,2,3,4,5,6]) = [4,5,6,1,2,3]
    */

    size_t neglen = W / 2;  // [4,5]
    size_t poslen = W - neglen;  // [1,2,3]

    #pragma omp parallel for
    for (size_t rID = 0; rID < H; ++rID) {
        auto outputrow = rID*W + output;
        auto inputrow = rID*W + input;
        std::memcpy(outputrow, inputrow + poslen, neglen*sizeof(T));
        std::memcpy(outputrow + neglen, inputrow, poslen*sizeof(T));
    }

}

SpectrumViewer::SpectrumViewer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SpectrumViewer)
{
    ui->setupUi(this);
    ui->freqaxis->parentGraphicsView = ui->graphicsView;
    ui->timeaxis->parentGraphicsView = ui->graphicsView;

    ui->graphicsView->scale(1,-1);

    QObject::connect(ui->graphicsView->verticalScrollBar(), &QScrollBar::valueChanged,
                     ui->freqaxis, &SpecViewer_AxisFreq::triggerUpdate);

    QObject::connect(ui->graphicsView->horizontalScrollBar(), &QScrollBar::valueChanged,
                     ui->timeaxis, &SpecViewer_AxisTime::triggerUpdate);
}

SpectrumViewer::~SpectrumViewer()
{
    delete ui;
    if (m_scene != nullptr)
    {
        delete m_scene;
    }
}


std::vector<float> SpectrumViewer::readdata(QStringList filepaths, int headeroffset)
{
    int bytesperrealsample = 2;

    std::vector<std::vector<char>> raw = std::vector<std::vector<char>>(filepaths.size());
    int totalsamples = 0;
    for (int fID=0; fID<filepaths.size(); fID++)
    {
        auto filepath = filepaths[fID].toStdString();
        std::ifstream myfile(filepath, std::ios::binary );
        if (!myfile) {
            std::cerr << "Failed to open the file: " << filepath << std::endl;
            continue;
        }
        myfile.seekg(0, std::ios::end);
        std::streamsize filesize_bytes = myfile.tellg();
        myfile.seekg(0, std::ios::beg);

        // Resize the vector to hold the file content
        std::vector<char> buffer(filesize_bytes);
        if (myfile.read(buffer.data(), filesize_bytes)) {
            raw[fID] = buffer;
        }
        else
        {
            std::cerr << "Read Error " << filepath << std::endl;
        }

        totalsamples += (filesize_bytes - headeroffset)/bytesperrealsample;
    }

    std::vector<float> fsignal(totalsamples);
    int j = 0;
    for (int vID=0; vID < raw.size(); vID++)
    {
        std::vector<char> v = raw[vID];
        # pragma omp simd
        for (int i=headeroffset; i<v.size(); i+=bytesperrealsample)
        {
            fsignal[j] = static_cast<float>(*reinterpret_cast<short*>(v.data() + i));
            j++;
        }
    }
    return fsignal;

}
fftwf_vector SpectrumViewer::makestft(std::vector<float>& data, bool isreal, int nfft, int stride)
{
    fftwf_vector outbuffer = fftwf_vector();
    if (!isreal)
    {
        int nsamples = data.size()/2;
        int ntimeslices = (nsamples - nfft)/stride + 1;
        fftwf_vector outbuffer_tmp(ntimeslices * nfft);
        int n[1] = {nfft};
        auto plan = fftwf_plan_many_dft(
            1, // int rank,
            n, //const int *n,
            ntimeslices, //int howmany,
            reinterpret_cast<fftwf_complex *>(data.data()), //fftw_complex *in,
            nullptr, //const int *inembed,
            1, // int istride,
            stride, // int idist,
            outbuffer_tmp.data(), //fftw_complex *out,
            nullptr, //const int *onembed,
            1, //int ostride,
            nfft, // int odist,
            FFTW_FORWARD, //int sign,
            FFTW_ESTIMATE); // unsigned flags);

        fftwf_execute(plan);
        fftwf_destroy_plan(plan);


        outbuffer = fftwf_vector(ntimeslices * nfft);
        fftshift(outbuffer_tmp.data(), outbuffer.data(), nfft, ntimeslices);

    }
    else
    {
        int nsamples = data.size();
        int ntimeslices = (nsamples - nfft)/stride + 1;
        int outnfft = (nfft) / 2 +  1;
        outbuffer = fftwf_vector(ntimeslices * outnfft);
        int n[1] = {nfft};
        auto plan = fftwf_plan_many_dft_r2c(
            1, // int rank,
            n, // const int *n,
            ntimeslices, // int howmany,
            data.data(), // double *in,
            nullptr,// const int *inembed,
            1, // int istride,
            nfft, // int idist,
            outbuffer.data(), // fftw_complex *out,
            nullptr, // const int *onembed,
            1, // int ostride,
            outnfft,// int odist,
            FFTW_ESTIMATE// unsigned flags
            );


        fftwf_execute(plan);
        fftwf_destroy_plan(plan);

    }

    return outbuffer;


}

void SpectrumViewer::quantizeSpectrogram(fftwf_vector& zxx, int ntime, int nfreq, float dynamicrange)
{
    // zxx is a C-contiguous matrix of height = ntime, width= nfreq


    std::vector<float> abs_zxx(zxx.size());
    std::complex<float> *zxx_ptr = reinterpret_cast<std::complex<float> *>(&zxx[0]);

    // take element wise abs of complex number
    #pragma omp simd
    for (int i=0; i<zxx.size(); ++i)
    {
        abs_zxx[i] = std::abs(zxx_ptr[i]);
    }

    // take element wise log
    std::vector<float> log_abs_zxx(zxx.size());
    #pragma omp simd
    for (int i=0; i<zxx.size(); ++i)
    {
        log_abs_zxx[i] = 10*std::log10(abs_zxx[i]);
    }

    // find max and min
    float max_float = FLT_MIN;
    float min_float = FLT_MIN;
    #pragma omp simd reduction(max:max_float) reduction(min:min_float)
    for (int i=0; i<log_abs_zxx.size(); ++i)
    {
        if (log_abs_zxx[i] > max_float)
        {
            max_float = log_abs_zxx[i];
        }
        if (log_abs_zxx[i] < min_float)
        {
            min_float = log_abs_zxx[i];
        }
    }

    float range = max_float - min_float;
    float min_float2 = min_float;

    // clip minimum
    if (range > dynamicrange)
    {
        min_float2 = max_float - dynamicrange;
        range = dynamicrange;
        #pragma omp simd
        for (int i=0; i<abs_zxx.size(); ++i)
        {
            if (log_abs_zxx[i] < min_float2)
            {
                log_abs_zxx[i] = min_float2;
            }
        }
    }


    // convert to uint8
    m_bytemap.resize(log_abs_zxx.size());
    #pragma omp simd
    for (int i=0; i<log_abs_zxx.size(); ++i)
    {
        m_bytemap[i] = static_cast<uchar>(255*(log_abs_zxx[i]-min_float2) / range);
    }

    // transpose
    transpose(m_bytemap, m_bytemap_transpose, nfreq, ntime);

}

void SpectrumViewer::display(fftwf_vector& zxx, int ntime, int nfreq)
{
    m_img = QImage(m_bytemap_transpose.data(), nfreq, ntime, QImage::Format::Format_Grayscale8);
    m_pix = QPixmap::fromImage(m_img);
    m_scene = new QGraphicsScene(this);
    m_scene->addPixmap(m_pix);

    ui->graphicsView->setScene(m_scene);
}

void SpectrumViewer::displaydata(QStringList filepaths, int headeroffset, bool isreal, int nfft, int stride, float dynamicrange)
{
    std::vector<float> iqdata =  readdata(filepaths, headeroffset);
    fftwf_vector zxx = makestft(iqdata, isreal, nfft, stride);


    int height, width;
    if (isreal)
    {
        height = nfft/2 + 1;
        width = (iqdata.size() - nfft) / stride + 1;
    }
    else
    {
        height = nfft;
        width = (iqdata.size()/2 - nfft) / stride + 1;
    }
    quantizeSpectrogram(zxx, width, height, dynamicrange);
    display(zxx,  height, width);

}


void SpectrumViewer::setFreqTimeRange(int startF, int stopF, QDateTime startT, QDateTime stopT)
{
    ui->freqaxis->global_startF = startF;
    ui->freqaxis->global_stopF = stopF;
    ui->timeaxis->global_startT = startT;
    ui->timeaxis->global_stopT = stopT;

}


