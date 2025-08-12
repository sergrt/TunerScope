#pragma once

#include "Settings.h"

#include <QAbstractListModel>

class TunerModel : public QAbstractListModel {
    Q_OBJECT

    enum Roles {
        FrequencyRole = Qt::UserRole + 10,
        NoteNameRole = Qt::UserRole + 11,
        NoteFrequencyRole = Qt::UserRole + 12,
        DeviationCents = Qt::UserRole + 13
    };

public:
    TunerModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void updateFromSettings(const Settings& settings);

public slots:
    void updateDetectedNotes(const QVector<float>& audioData);

private:
    struct TuneResult {
        QString noteName;
        float noteFreq{0.0f};
        float curFreq {0.0f};
        float cents{0.0f};
    };
    std::vector<TuneResult> m_maxNotes;
    std::vector<TuneResult> m_prevNotes;

    int m_fftSize{4096};
    int m_sampleRate{48000};
};
