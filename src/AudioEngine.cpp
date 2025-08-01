#include "AudioEngine.h"

#include <QAudioFormat>
#include <QMediaDevices>
#include <QtMath>
#include <QList>
#include <QTextStream>
#include <QDebug>

#include <fftw3.h>

#include <algorithm>

namespace {

void TraceDevices() {
    const QList<QAudioDevice> audioDevices = QMediaDevices::audioInputs();
    for (const QAudioDevice &device : audioDevices)
    {
        qDebug() << "ID: " << device.id() << Qt::endl;
        qDebug() << "Description: " << device.description() << Qt::endl;
        qDebug() << "Is default: " << (device.isDefault() ? "Yes" : "No") << Qt::endl;
        qDebug() << "Preffered sample rate: " << device.preferredFormat().sampleRate() << Qt::endl;
        qDebug() << "Preffered bytes per sample: " << device.preferredFormat().bytesPerFrame() << Qt::endl;
        qDebug() << "Preffered sample format: " << device.preferredFormat().sampleFormat() << Qt::endl;
        qDebug() << "Preffered channels count: " << device.preferredFormat().channelCount() << Qt::endl;

    }
}

} // namespace

AudioEngine::AudioEngine(QObject *parent)
    : QObject(parent) {
    TraceDevices();
}

AudioEngine::~AudioEngine() {
    m_audioInput->stop();
}

void AudioEngine::Start(const Settings& settings) {
    fftSize_ = settings.fftSize;
    sampleRate_ = settings.sampleRate;
    refreshRateMs_ = settings.refreshRateMs;
    initHannWindow();
    initPrevMagnitudes();

    QAudioFormat format{};
    format.setSampleRate(sampleRate_);
    format.setChannelCount(2);
    format.setSampleFormat(QAudioFormat::Float);
    format.setChannelConfig(QAudioFormat::ChannelConfigStereo);

    QAudioDevice device = QMediaDevices::defaultAudioInput();
    m_audioInput = new QAudioSource(device, format, this);
    m_inputDevice = m_audioInput->start();
    if (!m_inputDevice)
        throw std::runtime_error("Unable to start audio input");

    connect(&m_timer, &QTimer::timeout, this, &AudioEngine::processAudio);
    m_timer.start(refreshRateMs_);
}

void AudioEngine::restart(int fftSize) {
    m_timer.stop();
    m_audioInput->stop();

    fftSize_ = fftSize;
    initHannWindow();
    initPrevMagnitudes();

    QAudioFormat format{};
    format.setSampleRate(sampleRate_);
    format.setChannelCount(2);
    format.setSampleFormat(QAudioFormat::Float);
    format.setChannelConfig(QAudioFormat::ChannelConfigStereo);

    QAudioDevice device = QMediaDevices::defaultAudioInput();
    m_audioInput = new QAudioSource(device, format, this);
    m_inputDevice = m_audioInput->start();
    if (!m_inputDevice)
        throw std::runtime_error("Unable to start audio input");

    //connect(&m_timer, &QTimer::timeout, this, &AudioEngine::processAudio);
    m_timer.start(refreshRateMs_);
}

void AudioEngine::initHannWindow() {
    hannWindow_.resize(fftSize_);
    for (int i = 0; i < fftSize_; ++i) {
        hannWindow_[i] = 0.5f * (1.0f - qCos(2.0f * M_PI * i / (fftSize_ - 1)));
    }
}

void AudioEngine::initPrevMagnitudes() {
    prevMagnitudes_.resize(fftSize_ / 2, 0.0f);
}

void AudioEngine::processAudio()
{
    if (!m_inputDevice->bytesAvailable())
        return;

    auto data = m_inputDevice->readAll();
    //qDebug() << "Data size = " << data.size();

    // downmix to mono
    const float* samples = reinterpret_cast<const float*>(data.constData());
    int frameCount = data.size() / (sizeof(float) * 2); // 2 канала

    QVector<float> monoSamples;
    monoSamples.reserve(frameCount);

    for (int i = 0; i < frameCount; ++i) {
        float left = samples[i * 2];
        float right = samples[i * 2 + 1];
        float mono = 0.5f * (left + right);
        monoSamples.push_back(mono);
    }
    m_buffer = std::move(monoSamples);
    computeSpectrum(m_buffer);
}

void fft(std::vector<double[2]> &data, int fftSize) {
    fftw_complex *in, *out;
    fftw_plan p;
    const int N = fftSize; //data.size();

    //in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    //for (int i = 0; i < N; ++i) {
    //    in[i][0] = data[i].real();
    //    in[i][1] = data[i].imag();
    //}

    //out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    //p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    p = fftw_plan_dft_1d(N, static_cast<fftw_complex*>(data.data()), data.data(), FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(p); /* repeat as needed */

    fftw_destroy_plan(p);
    /*
    data.resize(N);
    for (int i = 0; i < N; ++i) {
        data[i] = std::complex<float>(out[i][0], out[i][1]);
    }
    */
    //fftw_free(in);



    //fftw_free(out);

    return;
    /*


    const size_t n = data.size();
    if (n <= 1) return;

    std::valarray<std::complex<float>> even = data[std::slice(0, n / 2, 2)];
    std::valarray<std::complex<float>>  odd = data[std::slice(1, n / 2, 2)];

    fft(even);
    fft(odd);

    for (size_t k = 0; k < n / 2; ++k) {
        std::complex<float> t = std::polar(1.0f, static_cast<float>(-2.0f * M_PI * k / n)) * odd[k];
        data[k] = even[k] + t;
        data[k + n/2] = even[k] - t;
    }
*/
}

void AudioEngine::computeSpectrum(const QVector<float> &buffer) {
    /*
    const int N = 4096;
    QVector<float> spectrum(N / 2);

    const qint16 *samples = reinterpret_cast<const qint16*>(buffer.constData());
    int sampleCount = buffer.size() / sizeof(qint16);

    for (int k = 0; k < N / 2; ++k) {
        float re = 0, im = 0;
        for (int n = 0; n < N && n < sampleCount; ++n) {
            float angle = 2.0 * M_PI * k * n / N;
            re += samples[n] * qCos(angle);
            im -= samples[n] * qSin(angle);
        }
        spectrum[k] = qSqrt(re * re + im * im);
    }

    emit spectrumUpdated(spectrum);
*/
    {
        //std::valarray<std::complex<float>> data(std::complex<float>(), fftSize);
        static std::vector<double[2]> data(fftSize_);
        for (int i = 0; i < fftSize_; ++i) {
            data[i][0] = 0.0;
            data[i][1] = 0.0;
        }


        for (int i = 0, sz = std::min(fftSize_, static_cast<int>(buffer.size())); i < sz; ++i) {
            data[i][0] = buffer[i] * hannWindow_[i];
            data[i][1] = 0.0;
        }

        //for (int j = 0; j < std::min(fftSize, static_cast<int>(buffer.size())); ++j) {
        //    data[j] = buffer[j] * m_window[j];
        //}
        fft(data, fftSize_);
        static QVector<float> magnitudes(fftSize_ / 2);  // Half of spectrum due to Nyquist frequency
        magnitudes.resize(0);

        for (int i = 0, sz = std::min(static_cast<int>(data.size()), fftSize_ / 2); i < sz; ++i) {
            //magnitudes.push_back(std::abs(data[i]));
            //magnitudes.push_back(sqrt( data[i].real() * data[i].real() + data[i].imag() * data[i].imag()));
            magnitudes.push_back(sqrt( data[i][0] * data[i][0] + data[i][1] * data[i][1]));
        }

        auto max_magnitude_iterator = std::max_element(magnitudes.begin(), magnitudes.end());
        auto max_magnitude = *max_magnitude_iterator;
        auto max_magnitude_freq = std::distance(magnitudes.begin(), max_magnitude_iterator) * static_cast<float>(sampleRate_) / fftSize_;
        /*
        int max_magnitude = 0;

        for (int i = 0; i < magnitudes.size(); ++i) {
            if (magnitudes[i] > max_magnitude) {
                max_magnitude = magnitudes[i];
            }
        }
        */

        {
            float max_magnitude = 0.0;
            float max_magnitude_freq = 0.0f;
            int max_idx = 0;
            for (int i = 0; i < magnitudes.size(); ++i) {
                auto freq = i * static_cast<float>(sampleRate_) / fftSize_;
                if (magnitudes[i] > max_magnitude && freq >= 80.0 && freq <= 3000.0) {
                    max_magnitude = magnitudes[i];
                    max_magnitude_freq = freq;
                    max_idx = i;
                }
            }
            //qDebug() << "Freq: " << max_magnitude_freq << " at " << max_idx;
        }

        // Normalize

        for (int i = 0; i < magnitudes.size(); ++i) {
            magnitudes[i] = magnitudes[i] / max_magnitude;
        }

        // Noise gate
        /*
        float threshold = 0.3;

        for (int i = 0; i < magnitudes.size(); ++i) {
            if (magnitudes[i] > threshold) {
                magnitudes[i] = 0.0f;
            }
        }
        */




        //m_prevMagnitudes.resize(magnitudes.size());

        for (int k = 0; k < magnitudes.size(); ++k) {
            //float norm = maxMag > 0.0f ? magnitudes[k] / maxMag : 0.0f;
            //norm = powf(norm, 0.4f);
            magnitudes[k] = 0.95f * prevMagnitudes_[k] + 0.05f * magnitudes[k];
            //magnitudes[k] = 0.5f * m_prevMagnitudes[k] + 0.5f * magnitudes[k];
            if (!qIsInf(magnitudes[k]))
                prevMagnitudes_[k] = magnitudes[k];
        }

        //emit spectrumPeaks(max_magnitude_freq);
        emit spectrumUpdated(magnitudes);
        return;

        // Convert scale to dB
        static QVector<float> magnitudesDB;
        magnitudesDB.reserve(magnitudes.size());
        magnitudesDB.resize(0);
        //std::fill(magnitudesDB.begin(), magnitudesDB.end(), 0.0f);

        float minDb = -80.0f;  // шумовой порог
        float maxDb = 0.0f;    // максимум (полная амплитуда)

        for (int i = 0, sz = magnitudes.size(); i < sz; ++i) {

            //float db = 20 * log10(magnitudes[i] + 1e-5f);
            //db = std::clamp(db, -80.0f, 0.0f);
            //float norm = powf((db + 80.0f) / 80.0f, 0.6f);

            // Перевод в дБ с защитой от лог(0)
            float dB = 20.0f * std::log10(std::max(magnitudes[i], 1e-6f));
            // Нормализация в [0.0 ... 1.0]
            float norm = std::clamp((dB - minDb) / (maxDb - minDb), 0.0f, 1.0f);

            magnitudesDB.push_back(norm);
        }




        emit spectrumUpdated(magnitudesDB);







        /*
        QVector<float> filtered;

        for (int k = 0; k < magnitudes.size(); ++k) {
            float freq = k * 48000.0 / fftSize;

            if (freq >= 80.0f && freq <= 3000.0f) {
                filtered.push_back(magnitudes[k]);
            }
        }






        emit spectrumUpdated(filtered);
        */
        return;
    }

    /*
    const float *samples = reinterpret_cast<const float*>(buffer.constData());
    int sampleCount = buffer.size() / sizeof(float);
    int bufferPosition = 0;
    for (int i = 0; i < sampleCount; ++i) {
        bufferPosition++;

        if (bufferPosition >= fftSize) {
            bufferPosition = 0;

            std::valarray<std::complex<float>> data(fftSize);
            for (int j = 0; j < fftSize; ++j) {
                data[j] = buffer[j] * m_window[j];
            }

            fft(data);

            QVector<float> magnitudes(fftSize / 2);
            float noiseThreshold = 0.002f;
            float maxMag = 0.0f;

            for (int k = 0; k < fftSize / 2; ++k) {
                float mag = std::abs(data[k]);
                float freq = float(k) * 48000 / fftSize;

                if (freq < 80.0f || freq > 3000.0f || mag < noiseThreshold) {
                    //mag = 0.0f;
                }

                if (mag > maxMag) maxMag = mag;
                magnitudes[k] = mag;
            }

            for (int k = 0; k < fftSize / 2; ++k) {
                float norm = maxMag > 0.0f ? magnitudes[k] / maxMag : 0.0f;
                norm = powf(norm, 0.4f);
                magnitudes[k] = 0.8f * m_prevMagnitudes[k] + 0.2f * norm;
                m_prevMagnitudes[k] = magnitudes[k];
            }

            emit spectrumUpdated(magnitudes);
        }
    }
    */
}
 
