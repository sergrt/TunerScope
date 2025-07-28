#include "ScaleModel.h"

#include "Globals.h"

ScaleModel::ScaleModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ScaleModel::rowCount(const QModelIndex &) const
{
    return m_scale.size();
}

QVariant ScaleModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != ScaleItemRole)
        return {};

    return m_scale.value(index.row());
}

QHash<int, QByteArray> ScaleModel::roleNames() const
{
    return {
        { ScaleItemRole, "scale_item" }
    };
}

void ScaleModel::updateScale(const QVector<float> &scale)
{
    //qDebug() << "Update scale:" << scale.size();

    QVector<int> scale_values;
    scale_values.push_back(0);
    for (int i = 1; i < scale.size(); ++i) {
        scale_values.push_back( i * static_cast<float>(kSampleRate) / kFftSize);
    }

    beginResetModel();
    m_scale = scale_values;
    endResetModel();
}

