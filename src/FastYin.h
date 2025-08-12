#pragma once

#include <QVector>

class FastYin {
public:
    FastYin(QVector<float> audioBuffer, int sampleRate, bool usePowerOf2Size = false);

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
    void differencePowerOf2();
    void cumulativeMeanNormalizedDifference();
    int absoluteThreshold();
    float parabolicInterpolation(int tauEstimate);

    QVector<float> m_audioBuffer;
    int m_sampleRate;


    QVector<float> m_yinBuffer;
    QVector<float> m_yinStyleACF;
    QVector<float> m_audioBufferFFT;
    QVector<float> m_kernel;
    double m_threshold{0.1};
    PitchDetectionResult m_result{};
    bool m_usePowerOf2Size{false};
};
