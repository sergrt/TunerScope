#include "TunerModel.h"

static const std::map<float, QString> noteMap = {
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

std::pair<float, QString> findClosestNote(float frequency) {
    if (noteMap.empty())
        return {};

    auto it = noteMap.lower_bound(frequency);

    if (it == noteMap.begin()) {
        return *it;
    } else if (it == noteMap.end()) {
        return *std::prev(it);
    } else {
        auto lower = std::prev(it);
        if ((frequency - lower->first) < (it->first - frequency)) {
            return *lower;
        } else {
            return *it;
        }
    }
}

TunerModel::TunerModel(const Settings& settings, QObject *parent)
    : QAbstractListModel(parent)
    , fftSize_{settings.fftSize}
    , sampleRate_{settings.sampleRate} {
}

int TunerModel::rowCount(const QModelIndex &) const {
    return max_notes.size();
}

QVariant TunerModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    if (role == FrequencyRole)
        return max_notes[index.row()].noteFreq;

    if (role == NoteNameRole) {
        return max_notes[index.row()].noteName;
    }

    return {};
}

QHash<int, QByteArray> TunerModel::roleNames() const {
    return {
        { FrequencyRole, "frequency" },
        { NoteNameRole, "noteName" },
    };
}

void TunerModel::updateDetectedNotes(const QVector<float> &spectrum) {
    auto max_magnitude_iterator = std::max_element(spectrum.begin(), spectrum.end());
    auto peakIndex = std::distance(spectrum.begin(), max_magnitude_iterator);
    auto max_magnitude_freq = std::distance(spectrum.begin(), max_magnitude_iterator) * static_cast<float>(sampleRate_) / fftSize_;

    float deltaF = static_cast<float>(sampleRate_) / fftSize_;

    // parabolic peak interpolation
    if (peakIndex > 0 && peakIndex < spectrum.size() - 1) {
        double y0 = spectrum[peakIndex - 1];
        double y1 = spectrum[peakIndex];
        double y2 = spectrum[peakIndex + 1];

        double delta = 0.5 * (y0 - y2) / (y0 - 2 * y1 + y2);
        double interpolatedIndex = peakIndex + delta;

        float frequency = interpolatedIndex * deltaF;

        auto closestNote = findClosestNote(max_magnitude_freq);
        float noteFreq = closestNote.first;
        float cents = 1200 * log2(frequency / noteFreq);


        // Apply prev result to smooth
        if (prev_result[0].noteName.isEmpty()) {
            prev_result[0] = {closestNote.second, noteFreq, frequency, cents};
        } else {
            max_notes[0] = {closestNote.second,
                            noteFreq,
                            0.8f * prev_result[0].curFreq + 0.2f * frequency,
                            0.8f * prev_result[0].cents + 0.2f * cents};
            prev_result[0] = max_notes[0];
        }

        //max_notes[0] = {closestNote.second, noteFreq, frequency, cents};
    }

    //emit dataChanged(createIndex(0,0), createIndex(fftSize_,0), {FrequencyRole, NoteNameRole});

    emit updateNote(max_notes[0].noteName,
                    max_notes[0].noteFreq,
                    max_notes[0].curFreq,
                    max_notes[0].cents);
}
