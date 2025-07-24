#include "SpectrumModel.h"
#include "globals.h"

SpectrumModel::SpectrumModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SpectrumModel::rowCount(const QModelIndex &) const
{
    return m_spectrum.size();
}

QVariant SpectrumModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::ToolTipRole)
        return index.row() * static_cast<float>(kSampleRate) / kFftSize;

    if (!index.isValid() || role != MagnitudeRole)
        return {};

    return m_spectrum.value(index.row());
}

QHash<int, QByteArray> SpectrumModel::roleNames() const
{
    return {
        { MagnitudeRole, "magnitude" },
        { Qt::ToolTipRole, "tooltip" }
    };
}

void SpectrumModel::updateSpectrum(const QVector<float> &spectrum)
{
    //qDebug() << "Update spectrum:" << spectrum.size();

    //QVector<float> normalized;
    //float maxVal = *std::max_element(spectrum.begin(), spectrum.end());

    /*
    if (maxVal > 1e-6f) {
        for (float v : spectrum) {
            // логарифмическая шкала: log1p нормализует от 0 до 1
            float norm = std::log1p(v) / std::log1p(maxVal);
            normalized.append(norm);
        }
    } else {
        normalized.fill(0.0f, spectrum.size());
    }
*/

    /*
    // remove noise
    const float noiseThreshold = 0.02f; // подбери по месту
    for (int i = 0; i < N; ++i) {
        if (magnitudes[i] < noiseThreshold)
            magnitudes[i] = 0.0f;
    }
    */
    beginResetModel();
    m_spectrum = spectrum;
    endResetModel();
}
 
