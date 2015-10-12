#include "graphics_objects/GraphicsRootItem.h"
#include "main_gui_objects/Model.h"
#include <algorithm>

Model::Model(QGraphicsScene *scene, QObject *parent) : QAbstractItemModel(parent), m_scene(scene) {
    m_rootItem = new GraphicsRootItem();
    m_scene->addItem(m_rootItem);

    QHash<Qt::ItemDataRole, QString> *dataHash;

    dataHash = new QHash<Qt::ItemDataRole, QString>();
    dataHash->insert(Qt::DisplayRole, "Name");
    dataHash->insert(Qt::ToolTipRole, "Your given name for the item. You can change it.");
    m_headHash.insert(NAME, *dataHash);

    dataHash = new QHash<Qt::ItemDataRole, QString>();
    dataHash->insert(Qt::DisplayRole, "Vis.");
    dataHash->insert(Qt::ToolTipRole, "The item can be visible or not, which is shown here.");
    m_headHash.insert(VISIBILITY, *dataHash);

    emit dataChanged(QModelIndex(), QModelIndex());
}

Model::~Model() {
}

QModelIndex Model::index(int row, int column, const QModelIndex &parentIndex) const {
    if(row < 0 || column < 0 || column > columnCount())
        return QModelIndex();
    GraphicsScheduleItem *parent = getInternItemFromIndex(parentIndex);
    if(parent && parent->numberOfChildren() > row) {
        return createIndex(row, column, parent->child(row));
    } else {
        return QModelIndex();
    }
}

QModelIndex Model::parent(const QModelIndex &index) const {
    GraphicsScheduleItem *item = getItemFromIndex(index);
    if(item) {
        GraphicsScheduleItem *parent = item->parent();
        if(parent == m_rootItem)
            return QModelIndex();
        else
            return createIndex(parent->childNumber(), 0, parent);
    }
    return QModelIndex();
}

bool Model::hasChildren(const QModelIndex &parentIndex) const {
    GraphicsScheduleItem *parent = getInternItemFromIndex(parentIndex);
    if(parent) {
        return (parent->numberOfChildren() > 0);
    } else {
        return false;
    }
}

int Model::rowCount(const QModelIndex &parentIndex) const {
    GraphicsScheduleItem *parent = getInternItemFromIndex(parentIndex);
    if(parent)
        return parent->numberOfChildren();
    else
        return 0;
}

int Model::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_headHash.size();
}

QVariant Model::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }
    GraphicsScheduleItem *item = getInternItemFromIndex(index);
    if(item) {
        if(index.column() == NAME && (role == Qt::DisplayRole || role == Qt::ToolTipRole)) {
            return item->name();
        } else
        if(index.column() == VISIBILITY && role == Qt::DecorationRole) {
            if(item->isVisible())
                return QIcon(":/images/EyeOpen.png");
            else
                return QIcon(":/images/EyeClosed.png");
        }
    }
    return QVariant();
}

bool Model::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role != Qt::EditRole)
        return false;
    GraphicsScheduleItem *item = getInternItemFromIndex(index);
    if(item) {
        if(index.column() == NAME) {
            item->setName(value.toString());
        } else {
            return false;
        }
    } else {
        return false;
    }
    emit dataChanged(index, index);
    return true;
}

bool Model::toggleValue(const QModelIndex &index) {
    GraphicsScheduleItem *item = getInternItemFromIndex(index);
    if(item) {
        if(index.column() == VISIBILITY)
            item->toggleVisibility();
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant Model::headerData(int column, Qt::Orientation orientation, int role) const {
    Q_UNUSED(orientation);
    if(column < 0 || column >= m_headHash.size())
        return QVariant();
    if(m_headHash.contains(Data(column)) && m_headHash[Data(column)].contains(Qt::ItemDataRole(role)))
        return m_headHash[Data(column)][Qt::ItemDataRole(role)];
    else
        return QVariant();
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const {
    if(index.isValid()) {
        if(index.column() == NAME)
            return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
        if(index.column() == VISIBILITY)
            return Qt::ItemIsUserCheckable | QAbstractItemModel::flags(index);
        return Qt::NoItemFlags;
    } else {
        return Qt::NoItemFlags;
    }
}

bool Model::addItem(GraphicsScheduleItem *newItem) {
    int lastRow = m_rootItem->numberOfChildren();
    beginInsertRows(QModelIndex(), lastRow, lastRow + 1);
    newItem->setParentItem(m_rootItem);
    endInsertRows();
    return true;
}

bool Model::remove(QModelIndex index) {
    if(!index.isValid())
        return false;
    GraphicsScheduleItem *item = getItemFromIndex(index);

    // m_rootItem must not be deleted, item must be valid and item needs to be a top level item!
    if(!item || item->parent() != m_rootItem) {
        return false;
    }
    emit rowsAboutToBeRemoved();
    beginRemoveRows(index.parent(), index.row(), index.row());
    m_scene->removeItem(item);
    delete item;
    endRemoveRows();
    emit rowsFinishedRemoval();
    return true;
}

bool Model::remove(QModelIndexList indices) {
    if(indices.empty())
        return true;
    QList<int> rowsToDelete;
    for(QModelIndex index : indices) {
        // Choose only one entry per row for the deletion – all have the same internal pointers
        if(index.column() == 0) {
            // Only first level items can be deleted, others are deleted with their parents
            if(index.parent() == QModelIndex()) {
                rowsToDelete.append(index.row());
            }
        }
    }
    if(rowsToDelete.empty())
        return true;
    // sort rows to first remove the once further "below"
    // otherwise wrong rows may be deleted as row/child numbers can change by deleting first the upper items
    std::sort(rowsToDelete.begin(), rowsToDelete.end()); //sorted ascending

    // iterate from back to front (descending) and delete rows and GraphicsScheduleItems
    bool allRemoved = true;
    emit rowsAboutToBeRemoved();
    QList<int>::iterator it = rowsToDelete.end();
    do {
        --it;
        allRemoved &= removeRow(*it);
    } while(it != rowsToDelete.begin());
    emit rowsFinishedRemoval();
    return allRemoved;
}

void Model::removeAll() {
    emit rowsAboutToBeRemoved();
    while(hasChildren()) {
        removeRow(rowCount() - 1);
    }
    emit rowsFinishedRemoval();
}

void Model::clearSelection() {
    m_rootItem->clearSelection();
}

GraphicsScheduleItem* Model::getItemFromIndex(const QModelIndex &index) const {
    GraphicsScheduleItem *item = getInternItemFromIndex(index);
    if(item == m_rootItem)
        return nullptr;
    return item;
}

QModelIndex Model::getIndexFromItem(GraphicsScheduleItem *item) const {
    return createIndex(item->childNumber(), 0, item);
}

void Model::endInsertRows() {
    QAbstractItemModel::endInsertRows();
}

bool Model::removeRow(int row) {
    QModelIndex indexx = index(row, 0, QModelIndex());
    GraphicsScheduleItem *item = getItemFromIndex(indexx);
    if(!item) // m_rootItem or not valid
        return false;
    beginRemoveRows(QModelIndex(), row, row);
    m_scene->removeItem(item);
    delete item;
    endRemoveRows();
    return true;
}

GraphicsScheduleItem* Model::getInternItemFromIndex(const QModelIndex &index) const {
    if(index.isValid()) {
        GraphicsScheduleItem *item = static_cast<GraphicsScheduleItem*>(index.internalPointer());
        if(item)
            return item;
    }
    return m_rootItem;
}
