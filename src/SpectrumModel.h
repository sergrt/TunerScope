#pragma once

#include <QAbstractListModel>

class SpectrumModel : public QAbstractListModel
{
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

public slots:
    void updateSpectrum(const QVector<float> &spectrum);

private:
    QVector<float> m_spectrum;
    QVector<int> m_scale;
};
