#pragma once

#include <QVector>
#include <chrono>

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

    int lastProcessingTimeMs() const {
        return m_lastProcessingTimeMs;
    }

private:
    void startPerformanceMeasure() {
        m_startTime = std::chrono::high_resolution_clock::now();
    }
    void stopPerformanceMeasure() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_startTime);
        m_lastProcessingTimeMs = duration.count();
    }
    std::chrono::high_resolution_clock::time_point m_startTime{};
    int m_lastProcessingTimeMs{0};


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
