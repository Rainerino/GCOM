#include "image_tagger.hpp"

image_tagger::image_tagger(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant image_tagger::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex image_tagger::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex image_tagger::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int image_tagger::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int image_tagger::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant image_tagger::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
