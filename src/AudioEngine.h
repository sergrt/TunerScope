#pragma once

#include "Settings.h"

#include <QObject>
#include <QAudioSource>
#include <QIODevice>
#include <QTimer>

class AudioEngine : public QObject
{
    Q_OBJECT
public:
    explicit AudioEngine(QObject *parent = nullptr);
    ~AudioEngine();

    void start();
    void stop();
    void updateSettings(const Settings& settings);

signals:
    //void spectrumPeaks(float freq);
    void spectrumUpdated(const QVector<float> &spectrum);

private slots:
    void processAudio();

public slots:
    void restart();

private:
    template<typename T>
    QVector<float> extractData(const QByteArray& src) {
        const T* samples = reinterpret_cast<const T*>(src.constData());
        int frameCount = src.size() / sizeof(T);
        if (m_channel == Settings::Channel::Both)
            frameCount = src.size() / (sizeof(T) * 2); // 2 channels

        QVector<float> monoSamples;
        monoSamples.reserve(frameCount);

        for (int i = 0; i < frameCount; ++i) {
            if (m_channel == Settings::Channel::Both) {
                T left = samples[i * 2];
                T right = samples[i * 2 + 1];
                float mono = 0.5f * (left + right);
                monoSamples.push_back(mono);
            } else {
                float mono = samples[i];
                monoSamples.push_back(mono);
            }
        }

        return monoSamples;
    }

    void initHannWindow();
    void initPrevMagnitudes();
    QAudioFormat composeAudioFormat() const;
    void computeSpectrum(const QVector<float> &buffer);

    QByteArray m_deviceId{};
    QAudioSource *m_audioInput = nullptr;
    QIODevice *m_inputDevice = nullptr;
    QVector<float> m_buffer;
    QTimer m_timer;

    QVector<float> m_hannWindow;
    QVector<float> m_prevMagnitudes;

    Settings::Channel m_channel{Settings::Channel::Left};
    int m_sampleRate{48000};
    QAudioFormat::SampleFormat m_sampleFormat{QAudioFormat::SampleFormat::Float};
    int m_fftSize{16384};
    int m_refreshRateMs{50};
};
 
