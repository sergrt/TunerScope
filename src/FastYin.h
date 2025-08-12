#pragma once

#include <QVector>

// Core implementation based on java implementation: https://github.com/JorenSix/TarsosDSP
// Code is modified in difference() part to use power of 2 FFT sizes and compared to
// conventional YIN algorithm to ensure correct results are retrived
class FastYin {
public:
    FastYin(QVector<float> audioBuffer, int sampleRate);

    float getPitch();

    struct PitchDetectionResult {
        float pitchHz{0.0f};
        float probability{0.0f};
        bool pitched{false};
    };

private:
    // Based on Java implementation: https://github.com/JorenSix/TarsosDSP/blob/master/core/src/main/java/be/tarsos/dsp/pitch/FastYin.java
    // Result is slightly different from canonical yin algorithm (probably due to fft sizes)
    // Stored here for reference
    void difference();

    // My implementation, produces exact the value as yin algorithm, uses FFT sizes of power of 2
    void differencePowerOf2();

    void cumulativeMeanNormalizedDifference();
    int absoluteThreshold();
    float parabolicInterpolation(int tauEstimate);

    QVector<float> m_audioBuffer;
    int m_sampleRate{44100};

    QVector<float> m_yinBuffer;
    QVector<float> m_yinStyleACF;
    QVector<float> m_audioBufferFFT;
    QVector<float> m_kernel;
    double m_threshold{0.05};
    PitchDetectionResult m_result{};
};
