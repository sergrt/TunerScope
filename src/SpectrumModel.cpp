#include "SpectrumModel.h"

SpectrumModel::SpectrumModel(const Settings& settings, QObject *parent)
    : QAbstractListModel(parent)
    , fftSize_{settings.fftSize}
    , sampleRate_{settings.sampleRate} {
}

void SpectrumModel::updateFft(int fftSize) {
    fftSize_ = fftSize;
}

int SpectrumModel::rowCount(const QModelIndex &) const {
    return spectrum_.size();
}

QVariant SpectrumModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    if (role == MagnitudeRole)
        return spectrum_.value(index.row());

    if (role == ScaleItemRole)
        return scaleValues_.value(index.row());

    return {};
}

QHash<int, QByteArray> SpectrumModel::roleNames() const {
    return {
        { MagnitudeRole, "magnitude" },
        { ScaleItemRole, "scaleItem" }
    };
}

void SpectrumModel::updateSpectrum(const QVector<float> &spectrum) {
    static int dbg = 0;
    ++dbg;
    if (dbg % 10 == 0)
        qDebug() << "Spectrum size: " << spectrum.size();
    const bool resetModel = prevSpectrumSize_ != spectrum.size();

    if (resetModel) {
        prevSpectrumSize_ = spectrum.size();
        beginResetModel();
    }

    spectrum_ = spectrum;

    if (resetModel) {
        scaleValues_.reserve(spectrum_.size());
        for (int i = 0, sz = spectrum_.size(); i < sz; ++i) {
            scaleValues_.push_back( i * static_cast<float>(sampleRate_) / fftSize_);
        }
    }

    if (resetModel) {
        endResetModel();
    } else {
        emit dataChanged(createIndex(0,0), createIndex(fftSize_,0), {MagnitudeRole, ScaleItemRole});
    }
}
