#pragma once

#include "Settings.h"

#include <QAbstractListModel>

class SpectrumModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        MagnitudeRole = Qt::UserRole + 1,
        ScaleItemRole = Qt::UserRole + 2
    };

    SpectrumModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void UpdateSettings(const Settings& settings);

public slots:
    void updateSpectrum(const QVector<float> &spectrum);

private:
    void ForceReset();

    QVector<float> spectrum_;
    int prevSpectrumSize_{0};
    QVector<int> scaleValues_;
    int fftSize_{4096};
    int sampleRate_{48000};
};
