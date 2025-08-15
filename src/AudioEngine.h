#pragma once

#include "Settings.h"

#include <QAudioSource>
#include <QIODevice>
#include <QObject>
#include <QTimer>

class AudioEngine : public QObject {
    Q_OBJECT

public:
    explicit AudioEngine(QObject *parent = nullptr);
    ~AudioEngine();

    void start();
    void stop();
    void updateFromSettings(const Settings& settings);

signals:
    void spectrumUpdated(const QVector<float>& spectrum);
    void audioBufferUpdated(const QVector<float>& audioData);

private slots:
    void processAudio();

public slots:
    void restart();

private:
    template<typename T>
    QVector<float> extractData(const QByteArray& src) {
        const T* samples = reinterpret_cast<const T*>(src.constData());
        if (!samples)
            return {};

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
    void computeSpectrum(QVector<float> buffer);

    std::unique_ptr<QAudioSource> m_audioInput;
    QIODevice* m_inputDevice{nullptr};
    QTimer m_timer;

    QVector<float> m_hannWindow;
    QVector<float> m_prevMagnitudes;

    QByteArray m_deviceId{};
    Settings::Channel m_channel{Settings::Channel::Both};
    int m_sampleRate{48000};
    QAudioFormat::SampleFormat m_sampleFormat{QAudioFormat::SampleFormat::Float};
    int m_fftSize{16384};
    int m_refreshRateMs{50};
};
