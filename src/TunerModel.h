#pragma once

#include "Settings.h"

#include <QAbstractListModel>

class TunerModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        FrequencyRole = Qt::UserRole + 10,
        NoteNameRole = Qt::UserRole + 11,
        NoteFrequencyRole = Qt::UserRole + 12,
        DeviationCents = Qt::UserRole + 13
    };

    TunerModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void UpdateSettings(const Settings& settings);

public slots:
    void updateDetectedNotes(const QVector<float> &spectrum);

private:
    struct TuneResult {
        QString noteName;
        float noteFreq = 0.0f;
        float curFreq = 0.0f;
        float cents = 0.0f;
    };
    std::vector<TuneResult> maxNotes_;
    std::vector<TuneResult> prevNotes_{};

    int fftSize_{4096};
    int sampleRate_{48000};
};
