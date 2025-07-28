#include "SpectrumModel.h"

SpectrumModel::SpectrumModel(const Settings& settings, QObject *parent)
    : QAbstractListModel(parent)
    , fftSize_{settings.fftSize}
    , sampleRate_{settings.sampleRate}
{
}

int SpectrumModel::rowCount(const QModelIndex &) const
{
    return m_spectrum.size();
}

QVariant SpectrumModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::ToolTipRole)
        //return index.row() * static_cast<float>(kSampleRate) / kFftSize;
        return m_scale.value(index.row());

    if (!index.isValid() || (role != MagnitudeRole && role != ScaleItemRole))
        return {};

    if (role == MagnitudeRole)
        return m_spectrum.value(index.row());

    if (role == ScaleItemRole) {


        return m_scale.value(index.row());
    }
    return {};
}

QHash<int, QByteArray> SpectrumModel::roleNames() const
{
    return {
        { MagnitudeRole, "magnitude" },
        { ScaleItemRole, "scaleItem" },
        { Qt::ToolTipRole, "tooltip" },
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

    static bool initialSet = false;
    if (initialSet == false) {
        beginResetModel();
    }

    m_spectrum = spectrum;
    if (initialSet == false) {
        endResetModel();
        initialSet = true;
    }

    QVector<int> scale_values;
    scale_values.push_back(0);
    for (int i = 1; i < m_spectrum.size(); ++i) {
        scale_values.push_back( i * static_cast<float>(sampleRate_) / fftSize_);
    }
    m_scale.swap(scale_values);

    //beginInsertRows(index(0,0), 0, kFftSize);
    dataChanged(createIndex(0,0), createIndex(fftSize_,0), {MagnitudeRole, ScaleItemRole, Qt::ToolTipRole});
    //endInsertRows();
}
 
