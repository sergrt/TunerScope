#pragma once

#include <QVector>

#include <chrono>

class YinPitchCalculator {
public:
    YinPitchCalculator(QVector<float> audioBuffer, int sampleRate);

    float getPitch();

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

    QVector<float> m_audioBuffer;
    int m_sampleRate{44100};

    typedef struct _Yin {
        int16_t bufferSize{0}; /**< Size of the audio buffer to be analysed */
        int16_t halfBufferSize{0}; /**< Half the buffer length */
        float* yinBuffer{nullptr}; /**< Buffer that stores the results of the intermediate processing steps of the algorithm */
        float probability{0.0f}; /**< Probability that the pitch found is correct as a decimal (i.e 0.85 is 85%) */
        float threshold {0.0f}; /**< Allowed uncertainty in the result as a decimal (i.e 0.15 is 15%) */
    } Yin;

    /**
     * Initialise the Yin pitch detection object
     * @param yin        Yin pitch detection object to initialise
     * @param bufferSize Length of the audio buffer to analyse
     * @param threshold  Allowed uncertainty (e.g 0.05 will return a pitch with ~95% probability)
     */
    void Yin_init(Yin *yin, int16_t bufferSize, float threshold);

    void Yin_free(Yin *yin);

    /**
     * Runs the Yin pitch detection algortihm
     * @param  yin    Initialised Yin object
     * @param  buffer Buffer of samples to analyse
     * @return        Fundamental frequency of the signal in Hz. Returns -1 if pitch can't be found
     */
    float Yin_getPitch(Yin *yin, float* buffer);

    /**
     * Certainty of the pitch found
     * @param  yin Yin object that has been run over a buffer
     * @return     Returns the certainty of the note found as a decimal (i.e 0.3 is 30%)
     */
    float Yin_getProbability(Yin *yin);

    void Yin_difference(Yin *yin, float* buffer);

    /**
     * Step 2: Calculate the cumulative mean on the normalised difference calculated in step 1
     * @param yin #Yin structure with information about the signal
     *      * This goes through the Yin autocorellation values and finds out roughly where shift is which
     * produced the smallest difference
     */
    void Yin_cumulativeMeanNormalizedDifference(Yin *yin);

    /**
     * Step 3: Search through the normalised cumulative mean array and find values that are over the threshold
     * @return Shift (tau) which caused the best approximate autocorellation. -1 if no suitable value is found over the threshold.
     */
    int16_t Yin_absoluteThreshold(Yin *yin);

    /**
     * Step 5: Interpolate the shift value (tau) to improve the pitch estimate.
     * @param  yin         [description]
     * @param  tauEstimate [description]
     * @return             [description]
     *      * The 'best' shift value for autocorellation is most likely not an interger shift of the signal.
     * As we only autocorellated using integer shifts we should check that there isn't a better fractional
     * shift value.
     */
    float Yin_parabolicInterpolation(Yin *yin, int16_t tauEstimate);
};
