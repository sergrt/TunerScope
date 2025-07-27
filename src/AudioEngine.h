#pragma once

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

signals:
    //void spectrumPeaks(float freq);
    void spectrumUpdated(const QVector<float> &spectrum);

private slots:
    void processAudio();

private:
    QAudioSource *m_audioInput = nullptr;
    QIODevice *m_inputDevice = nullptr;
    QVector<float> m_buffer;
    QTimer m_timer;

    void computeSpectrum(const QVector<float> &buffer);
	
	QVector<float> m_window;
    QVector<float> m_prevMagnitudes;
};
 
