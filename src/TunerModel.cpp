#include "TunerModel.h"

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

void TunerModel::updateDetectedNotes(const QVector<float> &spectrum) {
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
    qDebug() << "Estimated fundamental:" << result.frequency << "Hz, score:" << result.score;

    auto frequency = result.frequency;

    const auto closestNote = findClosestNote(frequency);
    const float noteFreq = closestNote.first;
    const float cents = 1200 * log2(frequency / noteFreq);

    if (m_prevNotes[1].noteName.isEmpty()) {
        m_prevNotes[1] = {closestNote.second, noteFreq, frequency, cents};
    } else {
        static const float kPrevNotesWeight = 0.7f;
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
