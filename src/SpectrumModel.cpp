#include "SpectrumModel.h"

SpectrumModel::SpectrumModel(QObject *parent)
    : QAbstractListModel(parent) {
}

void SpectrumModel::updateFromSettings(const Settings& settings) {
    m_sampleRate = settings.getSampleRate();
    m_fftSize = settings.getFftSize();
    forceReset();
}

void SpectrumModel::forceReset() {
    m_prevSpectrumSize = 0;
}

int SpectrumModel::rowCount(const QModelIndex &) const {
    return m_spectrum.size();
}

QVariant SpectrumModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    if (role == MagnitudeRole)
        return m_spectrum.value(index.row());

    if (role == ScaleItemRole)
        return m_scaleValues.value(index.row());

    return {};
}

QHash<int, QByteArray> SpectrumModel::roleNames() const {
    return {
        { MagnitudeRole, "magnitude" },
        { ScaleItemRole, "scaleItem" }
    };
}

void SpectrumModel::updateSpectrum(const QVector<float> &spectrum) {
    /*
    static int dbg = 0;
    ++dbg;
    if (dbg % 10 == 0)
        qDebug() << "Spectrum size: " << spectrum.size();
    */
    const bool resetModel = m_prevSpectrumSize != spectrum.size();

    if (resetModel) {
        m_prevSpectrumSize = spectrum.size();
        beginResetModel();
    }

    m_spectrum.resize(spectrum.size());

    // Normalize
    auto max_magnitude_iterator = std::max_element(spectrum.begin(), spectrum.end());
    auto max_magnitude = *max_magnitude_iterator;
    for (int i = 0; i < spectrum.size(); ++i) {
        m_spectrum[i] = spectrum[i] / max_magnitude;
    }

    if (resetModel) {
        m_scaleValues.reserve(m_spectrum.size());
        m_scaleValues.resize(0);
        for (int i = 0, sz = m_spectrum.size(); i < sz; ++i) {
            m_scaleValues.push_back(i * static_cast<float>(m_sampleRate) / m_fftSize);
        }
    }

    if (resetModel) {
        endResetModel();
    } else {
        emit dataChanged(createIndex(0,0), createIndex(m_fftSize,0), {MagnitudeRole, ScaleItemRole});
    }
}
