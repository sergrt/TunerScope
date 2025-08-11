#pragma once

#include <QVector>

class FastYin {
public:
    FastYin(QVector<float> audioBuffer, QVector<float> fftData, int sampleRate);

    float getPitch();

    struct PitchDetectionResult {
        /**
         * The pitch in Hertz.
         */
        float pitch{0.0f};
        float probability{0.0f};
        bool pitched{false};
    };

private:
    void difference();
    void cumulativeMeanNormalizedDifference();
    int absoluteThreshold();
    float parabolicInterpolation(int tauEstimate);

    QVector<float> m_audioBuffer;
    QVector<float> m_fftData;
    int m_sampleRate;


    QVector<float> m_yinBuffer;
    QVector<float> m_yinStyleACF;
    QVector<float> m_audioBufferFFT;
    QVector<float> m_kernel;
    double m_threshold{0.2};
    PitchDetectionResult m_result{};
};
