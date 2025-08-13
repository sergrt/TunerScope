#pragma once

#include "Settings.h"

#include <QAbstractListModel>

class SpectrumModel : public QAbstractListModel {
    Q_OBJECT

    enum Roles {
        MagnitudeRole = Qt::UserRole + 1,
        ScaleItemRole = Qt::UserRole + 2
    };

public:
    SpectrumModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void updateFromSettings(const Settings& settings);

public slots:
    void updateSpectrum(const QVector<float>& spectrum);

private:
    void forceReset();

    QVector<float> m_spectrum;
    int m_prevSpectrumSize{0};
    QVector<int> m_scaleValues;
    int m_fftSize{4096};
    int m_sampleRate{48000};
};
