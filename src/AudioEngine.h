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

    void Start(const Settings& settings);

signals:
    //void spectrumPeaks(float freq);
    void spectrumUpdated(const QVector<float> &spectrum);

private slots:
    void processAudio();

public slots:
    void restart(int fftSize);

private:
    void initHannWindow();
    void initPrevMagnitudes();

    QAudioSource *m_audioInput = nullptr;
    QIODevice *m_inputDevice = nullptr;
    QVector<float> m_buffer;
    QTimer m_timer;

    void computeSpectrum(const QVector<float> &buffer);

    QVector<float> hannWindow_;
    QVector<float> prevMagnitudes_;
    int fftSize_{16384};
    int sampleRate_{48000};
    int refreshRateMs_{50};
};
 
