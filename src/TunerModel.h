#pragma once

#include <QAbstractListModel>

class TunerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        FrequencyRole = Qt::UserRole + 10,
        NoteNameRole = Qt::UserRole + 11
    };

    TunerModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void updateDetectedNotes(const QVector<float> &spectrum);
signals:
    void updateNote(const QString& noteName, float noteFreq, float curFreq, float cents);

private:
    struct TuneResult {
        std::string noteName;
        float noteFreq = 0.0f;
        float curFreq = 0.0f;
        float cents = 0.0f;
    };
    std::vector<TuneResult> max_notes{{}};
    std::vector<TuneResult> prev_result{{}};
};
