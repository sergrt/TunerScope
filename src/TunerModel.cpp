#include "TunerModel.h"

#include "FastYin.h"
#include "Yin.h"

#include <iterator>

namespace {

static const std::vector<std::pair<float, QString>> noteMap = {
    {27.500f, "A0"},   {29.135f, "A#0"},  {30.868f, "B0"},
    {32.703f, "C1"},   {34.648f, "C#1"},  {36.708f, "D1"},   {38.891f, "D#1"},  {41.203f, "E1"},   {43.654f, "F1"},   {46.249f, "F#1"},  {48.999f, "G1"},   {51.913f, "G#1"},
    {55.000f, "A1"},   {58.270f, "A#1"},  {61.735f, "B1"},
    {65.406f, "C2"},   {69.296f, "C#2"},  {73.416f, "D2"},   {77.782f, "D#2"},  {82.407f, "E2"},   {87.307f, "F2"},   {92.499f, "F#2"},  {97.999f, "G2"},   {103.826f, "G#2"},
    {110.000f, "A2"},  {116.541f, "A#2"}, {123.471f, "B2"},
    {130.813f, "C3"},  {138.591f, "C#3"}, {146.832f, "D3"},  {155.563f, "D#3"}, {164.814f, "E3"},  {174.614f, "F3"},  {185.000f, "F#3"}, {195.998f, "G3"},  {207.652f, "G#3"},
    {220.000f, "A3"},  {233.082f, "A#3"}, {246.942f, "B3"},
    {261.626f, "C4"},  {277.183f, "C#4"}, {293.665f, "D4"},  {311.127f, "D#4"}, {329.628f, "E4"},  {349.228f, "F4"},  {369.994f, "F#4"}, {391.995f, "G4"},  {415.305f, "G#4"},
    {440.000f, "A4"},  {466.164f, "A#4"}, {493.883f, "B4"},
    {523.251f, "C5"},  {554.365f, "C#5"}, {587.330f, "D5"},  {622.254f, "D#5"}, {659.255f, "E5"},  {698.456f, "F5"},  {739.989f, "F#5"}, {783.991f, "G5"},  {830.609f, "G#5"},
    {880.000f, "A5"},  {932.328f, "A#5"}, {987.767f, "B5"},
    {1046.502f, "C6"}, {1108.731f, "C#6"},{1174.659f, "D6"}, {1244.508f, "D#6"},{1318.510f, "E6"}, {1396.913f, "F6"}, {1479.978f, "F#6"},{1567.982f, "G6"}, {1661.219f, "G#6"},
    {1760.000f, "A6"}, {1864.655f, "A#6"},{1975.533f, "B6"},
    {2093.005f, "C7"}, {2217.461f, "C#7"},{2349.318f, "D7"}, {2489.016f, "D#7"},{2637.020f, "E7"}, {2793.826f, "F7"}, {2959.955f, "F#7"},{3135.963f, "G7"}, {3322.438f, "G#7"},
    {3520.000f, "A7"}, {3729.310f, "A#7"},{3951.066f, "B7"},
    {4186.009f, "C8"}
};

auto findClosestNoteIterator(float frequency) {
    float minDiff = std::numeric_limits<float>::max();
    int maxIndex = 0;

    size_t i = 0;
    while (i < noteMap.size()) {
        const auto& [freq, name] = noteMap[i];
        const float diff = std::abs(freq - frequency);
        if (diff < minDiff) {
            minDiff = diff;
            maxIndex = i;
        }
        ++i;
    };

    auto closestIterator = noteMap.begin();
    std::advance(closestIterator, maxIndex);
    return closestIterator;
}

std::pair<float, QString> findClosestNote(float frequency) {
    if (noteMap.empty())
        return {};

    return *findClosestNoteIterator(frequency);
}

std::pair<float, QString> findPrevNote(float frequency) {
    const auto it = findClosestNoteIterator(frequency);
    if (it != noteMap.begin()) {
        return *std::prev(it);
    }

    return {0.0f, QString("-")};
}

std::pair<float, QString> findNextNote(float frequency) {
    const auto it = findClosestNoteIterator(frequency);
    if (it != noteMap.end()) {
        auto next = std::next(it);
        if (next != noteMap.end())
            return *next;
    }

    return {0.0f, QString("-")};
}

struct PitchEstimate {
    float frequency;   // Оценённая фундаментальная частота
    float score;       // Общая "сила" гармоник

    int bestK = 1;          // какая гармоника использована (k)
    int peakBin = 0;        // бин первоначального пика
};

float parabolicInterpolation(const QVector<float>& spectrum, int bin, float& interpolatedAmplitude) {
    if (bin <= 0 || bin >= spectrum.size() - 1) {
        interpolatedAmplitude = spectrum[bin];
        return 0.0f;
    }

    const float y1 = spectrum[bin - 1];
    const float y2 = spectrum[bin];
    const float y3 = spectrum[bin + 1];

    const float denom = y1 - 2 * y2 + y3;
    if (qFuzzyCompare(denom, 0.0f)) {
        interpolatedAmplitude = y2;
        return 0.0f;
    }

    const float dx = 0.5f * (y1 - y3) / denom;
    interpolatedAmplitude = y2 - 0.25f * (y1 - y3) * dx;
    return dx; // difference in range [-0.5..+0.5]
}
/*
PitchEstimate estimateFundamentalHarmonicSum(const QVector<float>& spectrum, float sampleRate, int fftSize, int maxHarmonic = 5, int minBin = 1, int maxBin = -1) {
    if (spectrum.isEmpty())
        return PitchEstimate{0.0f, 0.0f};

    if (maxBin < 0 || maxBin >= spectrum.size())
        maxBin = spectrum.size() - 1;

    // Find peak
    const int peakBin = std::distance(spectrum.begin(), std::max_element(spectrum.begin() + minBin, spectrum.begin() + maxBin));
    const float peakFrequency = (sampleRate * peakBin) / fftSize;

    PitchEstimate best = { peakFrequency, 0.0f };

    for (int k = 1; k <= maxHarmonic; ++k) {
        const float f0 = peakFrequency / k;
        if (f0 < 20.0f)
            continue;

        float score = 0.0f;

        for (int h = 1; h <= maxHarmonic; ++h) {
            const float harmonicFreq = f0 * h;
            const float exactBin = harmonicFreq * fftSize / sampleRate;
            const int bin = qRound(exactBin);

            if (bin <= 0 || bin >= spectrum.size() - 1)
                continue;

            float interpolatedAmplitude{0.0f};
            float dx = parabolicInterpolation(spectrum, bin, interpolatedAmplitude);
            Q_UNUSED(dx);
            //qDebug() << "dx: " << dx;
            const float harmonicWeight = 1.0f / h;

            score += interpolatedAmplitude * harmonicWeight;
        }

        if (score > best.score) {
            best.frequency = f0;
            best.score = score;
        }
    }

    return best;
}
*/
PitchEstimate estimateFundamentalHarmonicSum(
    const QVector<float> &spectrum,
    double sampleRate,
    int fftSize,
    int maxHarmonics = 6,
    double minFreq = 40.0,
    double maxFreq = 1000.0,
    bool debug = false)
{
    PitchEstimate result;
    if (spectrum.isEmpty() || fftSize <= 0 || sampleRate <= 0.0) return result;

    const int specSize = spectrum.size();            // обычно fftSize/2
    // Преобразуем частотные границы в бины (ограничиваем в пределах спектра)
    int binMin = qMax(1, int(std::floor(minFreq * fftSize / sampleRate)));
    int binMax = int(std::ceil(maxFreq * fftSize / sampleRate));
    binMax = qMin(binMax, specSize - 1);
    if (binMin >= binMax) {
        // Неправильные входные параметры
        if (debug) qDebug() << "estimateFundamental: invalid bin range" << binMin << binMax;
        return result;
    }

            // Находим локальный сильнейший пик в диапазоне (начальная точка)
    int peakBin = binMin;
    {
        auto it = std::max_element(spectrum.begin() + binMin, spectrum.begin() + binMax + 1);
        peakBin = std::distance(spectrum.begin(), it);
    }
    double peakFreq = (double)peakBin * sampleRate / fftSize;
    if (debug) qDebug() << "initial peakBin" << peakBin << "peakFreq" << peakFreq;

            // Ограничение на k: не будем делить на k, если f0 окажется < minFreq
    int maxK = std::min(maxHarmonics, int(std::floor(peakFreq / minFreq)));
    maxK = qMax(1, maxK);

    double bestScore = -1.0;
    int bestK = 1;
    double bestF0 = peakFreq;

            // Для каждого кандидата k (предположение: peakBin = k * f0_bin)
    for (int k = 1; k <= maxK; ++k) {
        double f0 = peakFreq / k;
        if (f0 < minFreq || f0 > maxFreq) continue;

        double score = 0.0;
        // пробегаем гармоники
        for (int h = 1; h <= maxHarmonics; ++h) {
            double hFreq = f0 * h;
            double exactBin = hFreq * fftSize / sampleRate;
            int bin = int(std::floor(exactBin + 0.5)); // ближайший бин

            if (bin <= 0 || bin >= specSize - 1) break; // вне спектра

            float amp;
            double dx = parabolicInterpolation(spectrum, bin, amp);
            // Можно учитывать небольшое смещение: refinedBin = bin + dx
            // weight: уменьшаем вклад старших гармоник
            double weight = 1.0 / h;
            score += amp * weight;
        }

        if (score > bestScore) {
            bestScore = score;
            bestK = k;
            bestF0 = f0;
        }
    }

            // Если ничего не подобралось (вдруг), вернём пиковую
    if (bestScore < 0.0) {
        bestScore = spectrum[peakBin];
        bestF0 = peakFreq;
        bestK = 1;
    }

            // Финальная доработка: уточним bestF0 через интерполяцию вокруг соответствующего бина
            // найдем бин для bestF0:
    double bestExactBin = bestF0 * fftSize / sampleRate;
    int bestBin = int(std::floor(bestExactBin + 0.5));
    float finalAmp = 0.0;
    double dxFinal = 0.0;
    if (bestBin > 0 && bestBin < specSize - 1) {
        dxFinal = parabolicInterpolation(spectrum, bestBin, finalAmp);
    }
    double refinedF0 = (bestBin + dxFinal) * sampleRate / fftSize;

    result.frequency = refinedF0;
    result.score = bestScore;
    result.bestK = bestK;
    result.peakBin = peakBin;

    if (debug) {
        qDebug() << "estimateFundamental: bestK" << bestK
                 << "peakBin" << peakBin
                 << "bestF0(orig)" << bestF0
                 << "bestBin" << bestBin
                 << "dxFinal" << dxFinal
                 << "refinedF0" << refinedF0
                 << "score" << bestScore;
    }

    return result;
}

float detectPitchYin(const QVector<float>& signal, float sampleRate) {
    const int frameSize = signal.size();
    const int tauMin = 10; // Minimum period (in samples)
    const int tauMax = frameSize / 2; // Maximum period (in samples)

    QVector<float> yinBuffer(tauMax - tauMin + 1);
    QVector<float> cumulativeMeanNormalizedDifference(tauMax - tauMin + 1);

    // Calculate the cumulative mean normalized difference
    for (int tau = tauMin; tau <= tauMax; tau++) {
        float sum = 0.0f;
        for (int i = 0; i < frameSize - tau; i++) {
            float diff = signal[i] - signal[i + tau];
            sum += diff * diff;
        }
        yinBuffer[tau - tauMin] = sum / (frameSize - tau);
    }

    // Calculate the cumulative mean normalized difference
    cumulativeMeanNormalizedDifference[0] = 1.0f;
    for (int tau = tauMin + 1; tau <= tauMax; tau++) {
        cumulativeMeanNormalizedDifference[tau - tauMin] = yinBuffer[tau - tauMin] / (yinBuffer[tau - tauMin - 1] + yinBuffer[tau - tauMin]);
    }

    /*
    qDebug() << "Tau range: [" << tauMin << ", " << tauMax << "]";
    qDebug() << "CMN function:";
    for (int i = 0; i < cumulativeMeanNormalizedDifference.size(); i++) {
        qDebug() << "  " << i << ": " << cumulativeMeanNormalizedDifference[i] << "\n";
    }
    */

    // Find the minimum value in the cumulative mean normalized difference
    int minIndex = 0;
    float minValue = 1.0f;
    for (int i = 0; i < cumulativeMeanNormalizedDifference.size(); i++) {
        if (cumulativeMeanNormalizedDifference[i] < minValue) {
            minValue = cumulativeMeanNormalizedDifference[i];
            minIndex = i;
        }
    }

    // Calculate the detected pitch frequency
    float pitchFrequency = sampleRate / (tauMin + minIndex);

    return pitchFrequency;
}

QVector<float> calculateCmnFunction(const QVector<float>& signal) {
    const int tauMin = 10; // Adjust this value as needed
    const int tauMax = signal.size() *0.9; // Adjust this value as needed

    QVector<float> cmnFunction(tauMax - tauMin + 1);

    for (int tau = tauMin; tau <= tauMax; tau++) {
        float sum = 0.0f;
        for (int i = 0; i < signal.size() - tau; i++) {
            float diff = std::abs(signal[i] - signal[i + tau]);
            sum += diff;
        }
        cmnFunction[tau - tauMin] = sum / (signal.size() - tau);
    }

            // Calculate the cumulative mean normalized difference
    for (int i = 1; i < cmnFunction.size(); i++) {
        cmnFunction[i] = (cmnFunction[i] + cmnFunction[i-1]) / 2.0f;
    }

    return cmnFunction;
}
int findMinIndex(const QVector<float>& cmnFunction, float threshold) {
    int minIndex = -1;
    float minValue = std::numeric_limits<float>::max();

    for (int i = 0; i < cmnFunction.size(); i++) {
        if (cmnFunction[i] < minValue && cmnFunction[i] < threshold) {
            minValue = cmnFunction[i];
            minIndex = i;
        }
    }

    return minIndex;
}
float calculatePitchPeriod(const QVector<float>& signal, float sampleRate) {
    QVector<float> autocorrelation(signal.size());
    /*
    for (int i = 0; i < signal.size(); i++) {
        autocorrelation[i] = 0;
        for (int j = 0; j < signal.size() - i; j++) {
            autocorrelation[i] += signal[j] * signal[j + i];
        }
    }
    */
    autocorrelation = signal;

    int minIndex = 0;
    float minValue = std::numeric_limits<float>::max();
    for (int i = 1; i < autocorrelation.size(); i++) {
        if (autocorrelation[i] < minValue) {
            minValue = autocorrelation[i];
            minIndex = i;
        }
    }

    return (minIndex + 1) / sampleRate;
}

float calculatePitchFrequency(float pitchPeriod) {
    return 1.0f / pitchPeriod;
}
QVector<float> applyLowPassFilter(const QVector<float>& signal, float sampleRate) {
    const float cutoffFrequency = 8000.0f; // Adjust this value as needed
    const float rc = 1.0f / (2.0f * 3.14159f * cutoffFrequency);
    const float dt = 1.0f / sampleRate;

    QVector<float> filteredSignal(signal.size());
    filteredSignal[0] = signal[0];

    for (int i = 1; i < signal.size(); i++) {
        filteredSignal[i] = (signal[i] + filteredSignal[i-1] * (1.0f - dt / rc)) / (1.0f + dt / rc);
    }

    return filteredSignal;
}
QVector<float> applySmoothingFilter(const QVector<float>& cmnFunction) {
    const int windowSize = 5; // Adjust this value as needed

    QVector<float> smoothedCmnFunction(cmnFunction.size());

    for (int i = 0; i < cmnFunction.size(); i++) {
        float sum = 0.0f;
        for (int j = std::max(0, i - windowSize / 2); j <= qMin(cmnFunction.size() - 1, i + windowSize / 2); j++) {
            sum += cmnFunction[j];
        }
        smoothedCmnFunction[i] = sum / windowSize;
    }

    return smoothedCmnFunction;
}
float detectPitchYin1(const QVector<float>& signal, float sampleRate) {
    // ...

    // Adjust the threshold values for the normalized signal
    float threshold = 0.001f; // Adjust this value as needed

    QVector<float> filteredSignal = applyLowPassFilter(signal, sampleRate);
    QVector<float> smoothedCmnFunction = applySmoothingFilter(filteredSignal);
    // Calculate the CMN function
    QVector<float> cmnFunction = calculateCmnFunction(signal);
    

    // Find the minimum value of the CMN function
    int minIndex = findMinIndex(cmnFunction, threshold);

    // Calculate the pitch period
    float pitchPeriod = calculatePitchPeriod(cmnFunction, sampleRate);

    // Calculate the pitch frequency
    float pitchFrequency = calculatePitchFrequency(pitchPeriod);

    // Print the pitch frequency
    qDebug() << "Pitch frequency: " << pitchFrequency;
    return pitchFrequency;
}



} // namespace

TunerModel::TunerModel(QObject *parent)
    : QAbstractListModel(parent) {

    m_maxNotes.resize(3); // [prev note] [cur note] [next note]
    m_prevNotes.resize(3);
}

void TunerModel::updateFromSettings(const Settings& settings) {
    m_fftSize = settings.getFftSize();
    m_sampleRate = settings.getSampleRate();
}

int TunerModel::rowCount(const QModelIndex &) const {
    return m_maxNotes.size();
}

QVariant TunerModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    if (role == FrequencyRole)
        return m_maxNotes[index.row()].curFreq;

    if (role == NoteNameRole) {
        return m_maxNotes[index.row()].noteName;
    }

    if (role == NoteFrequencyRole) {
        return m_maxNotes[index.row()].noteFreq;
    }

    if (role == DeviationCents) {
        return m_maxNotes[index.row()].cents;
    }

    return {};
}

QHash<int, QByteArray> TunerModel::roleNames() const {
    return {
        { FrequencyRole, "curFreq" },
        { NoteNameRole, "noteName" },
        { NoteFrequencyRole, "noteFreq" },
        { DeviationCents, "cents" }
    };
}

void TunerModel::updateDetectedNotes(const QVector<float> &audioData, const QVector<float> &spectrum) {
    /*
    const auto max_magnitude_iterator = std::max_element(spectrum.begin(), spectrum.end());
    const auto peakIndex = std::distance(spectrum.begin(), max_magnitude_iterator);
    const auto max_magnitude_freq = std::distance(spectrum.begin(), max_magnitude_iterator) * static_cast<float>(m_sampleRate) / m_fftSize;

    const float deltaF = static_cast<float>(m_sampleRate) / m_fftSize;

    // parabolic peak interpolation
    if (peakIndex > 0 && peakIndex < spectrum.size() - 1) {
        const double y0 = spectrum[peakIndex - 1];
        const double y1 = spectrum[peakIndex];
        const double y2 = spectrum[peakIndex + 1];

        const double delta = 0.5 * (y0 - y2) / (y0 - 2 * y1 + y2);
        const double interpolatedIndex = peakIndex + delta;

        const float frequency = interpolatedIndex * deltaF;

        const auto closestNote = findClosestNote(frequency);
        const float noteFreq = closestNote.first;
        const float cents = 1200 * log2(frequency / noteFreq);

        // Apply prev result to smooth
        if (m_prevNotes[1].noteName.isEmpty()) {
            m_prevNotes[1] = {closestNote.second, noteFreq, frequency, cents};
        } else {
            m_maxNotes[1] = {closestNote.second,
                            noteFreq,
                            0.7f * m_prevNotes[1].curFreq + 0.3f * frequency,
                            0.7f * m_prevNotes[1].cents + 0.3f * cents};
            / *
            maxNotes_[1] = {closestNote.second,
                            noteFreq,
                            frequency,
                            cents};
            * /
            m_prevNotes[1] = m_maxNotes[1];
        }

        auto prev = findPrevNote(frequency);
        m_maxNotes[0].noteFreq = prev.first;
        m_maxNotes[0].noteName = prev.second;

        auto next = findNextNote(frequency);
        m_maxNotes[2].noteFreq = next.first;
        m_maxNotes[2].noteName = next.second;
    }
    */

    auto result = estimateFundamentalHarmonicSum(spectrum, m_sampleRate, m_fftSize);
    //qDebug() << "Estimated fundamental:" << result.frequency << "Hz, score:" << result.score;

    //auto frequency = result.frequency;
    //float frequency = detectPitchYin(spectrum, m_sampleRate);

    // Compare yin results
    YinPitchCalculator yin(audioData, m_sampleRate);
    float freqYin = yin.getPitch();
    FastYin fastYin(audioData, m_sampleRate, false);
    float freqFastYin = fastYin.getPitch();
    FastYin fastYin2(audioData, m_sampleRate, true);
    float freqFastYin2 = fastYin2.getPitch();

    qDebug() << "Detected frequency: " << "yin = " << freqYin << ", fastYin = " << freqFastYin << ", fastYin2 = " << freqFastYin2;

    float frequency = freqYin;
    //FastYin fastYin(audioData, m_sampleRate);
    //float frequency = fastYin.getPitch();
    qDebug() << "Detect frequency: " << frequency;

    const auto closestNote = findClosestNote(frequency);
    const float noteFreq = closestNote.first;
    float cents = 1200 * log2(frequency / noteFreq);
    if (!std::isfinite(cents))
        cents = 0.0f;
    qDebug() << "Cents = " << cents;
    if (m_prevNotes[1].noteName.isEmpty()) {
        m_prevNotes[1] = {closestNote.second, noteFreq, frequency, cents};
    } else {
        static const float kPrevNotesWeight = 0.88f;
        m_maxNotes[1] = {closestNote.second,
                         noteFreq,
                         kPrevNotesWeight * m_prevNotes[1].curFreq + (1.0f - kPrevNotesWeight) * frequency,
                         kPrevNotesWeight * m_prevNotes[1].cents + (1.0f - kPrevNotesWeight) * cents};
        m_prevNotes[1] = m_maxNotes[1];
    }

    auto prev = findPrevNote(frequency);
    m_maxNotes[0].noteFreq = prev.first;
    m_maxNotes[0].noteName = prev.second;

    auto next = findNextNote(frequency);
    m_maxNotes[2].noteFreq = next.first;
    m_maxNotes[2].noteName = next.second;

    emit dataChanged(createIndex(0,0), createIndex(3,0), {FrequencyRole, NoteNameRole, NoteFrequencyRole, DeviationCents});
}
