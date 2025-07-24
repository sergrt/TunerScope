#pragma once

#include <QAbstractListModel>

class ScaleModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        ScaleItemRole = Qt::UserRole + 2
    };

    ScaleModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void updateScale(const QVector<float> &scale);

private:
    QVector<int> m_scale;
};
