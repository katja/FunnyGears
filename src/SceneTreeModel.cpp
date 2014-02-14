#include "SceneTreeModel.h"

SceneTreeModel::SceneTreeModel(QObject *parent) : QAbstractItemModel(parent) {

    m_headerData.append(new QVariant("Visibility"));
    m_visibilityColumn = 0;
    m_headerData.append(new QVariant("Name"));
    m_nameColumn = 1;

    m_rootItem = new SceneTreeItem();

    // emit layoutChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);
    emit dataChanged(QModelIndex(), QModelIndex());
}

SceneTreeModel::~SceneTreeModel() {
}

SceneTreeItem* SceneTreeModel::setRoot(SceneTreeItem *rootItem) {
    SceneTreeItem *oldRoot = m_rootItem;
    emit layoutAboutToBeChanged();
    m_rootItem = rootItem;
    //read QAbstractItemModel - layoutChanged()
    //maybe more is necessary here!!!
    emit layoutChanged();
    return oldRoot;
}

QModelIndex SceneTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if(row < 0)
        return QModelIndex();
    SceneTreeItem *parentItem = findItemBy(parent);
    if(parentItem && parentItem->numberOfChildren() > row) {
        return createIndex(row, column, parentItem->child(row));
    } else {
        return QModelIndex();
    }
}

/** Returns th parent of the model item with the given index.
 *  If the item has no parent, an invalid QModelIndex is returned.
 *
 *  A common convention is that only the items in the first column have children.
 *  Therefore the column of the returned QModelIndex would always be 0.
 */
QModelIndex SceneTreeModel::parent(const QModelIndex &index) const {
    SceneTreeItem *childItem = findItemBy(index);
    if(childItem) {
        if(childItem == m_rootItem | childItem->parent() == m_rootItem)
            return QModelIndex();
        else {
            SceneTreeItem *parentItem = childItem->parent();
            return createIndex(parentItem->childNumber(), 0, parentItem);
        }
    }
    return QModelIndex();
} //When reimplementing this function in a subclass, be careful to avoid calling QModelIndex member functions, such as QModelIndex::parent(), since indexes belonging to your model will simply call your implementation, leading to infinite recursion.

bool SceneTreeModel::hasChildren(const QModelIndex &parent) const {
    SceneTreeItem *parentItem = findItemBy(parent);
    if(!parentItem)
        return false;
    else
        return (parentItem->numberOfChildren() > 0);
}

int SceneTreeModel::rowCount(const QModelIndex &parent) const {
    SceneTreeItem *parentItem = findItemBy(parent);
    if(parentItem)
        return parentItem->numberOfChildren();
    else
        return 0;
}

int SceneTreeModel::columnCount(const QModelIndex &parent) const {
    return m_headerData.size();
}

QVariant SceneTreeModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid())
        return QVariant();
    SceneTreeItem *item = findItemBy(index);
    if(item) {
        if(role == Qt::DisplayRole || role == Qt::ToolTipRole) { //TODO: do something more suitable for ToolTipRole
            if(index.column() == m_nameColumn)
                return item->name();
            if(index.column() == m_visibilityColumn)
                return item->isVisible();
        }
    }
    return QVariant();
}

bool SceneTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role != Qt::EditRole)
        return false;
    SceneTreeItem *item = findItemBy(index);
    if(item) {
        if(index.column() == m_visibilityColumn) {
            item->toggleVisibility();
        } else if (index.column() == m_nameColumn) {
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

QVariant SceneTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(section < 0 || section >= m_headerData.size())
        return QVariant();
    if(role == Qt::ToolTipRole){
        return *(m_headerData[section]); //TODO: use something suitable for tool tips here!
    } else if(role == Qt::DisplayRole) {
        return *(m_headerData[section]);
    } else {
        return QVariant();
    }
}

Qt::ItemFlags SceneTreeModel::flags(const QModelIndex &index) const {
    if(index.isValid()) {
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    } else {
        return Qt::NoItemFlags;
    }
}

bool SceneTreeModel::addItem() {
    int lastRow = rowCount(QModelIndex());
    return insertRows(lastRow, 1, QModelIndex());
}

bool SceneTreeModel::remove(QModelIndex index) {
    if(!index.isValid())
        return false;
    return removeRows(index.row(), 1, index.parent());
}

bool SceneTreeModel::insertRows(int row, int count, const QModelIndex &parent) {
    if(parent.isValid() || row < 0 || row > m_rootItem->numberOfChildren())
        return false;

    SceneTreeItem *parentItem = findItemBy(parent);
    if(!parentItem)
        return false;

    beginInsertRows(parent, row, count);
    for(int i = 0; i < count; ++i) {
        parentItem->addChild();
    }
    endInsertRows();
    return true;
}

bool SceneTreeModel::removeRows(int row, int count, const QModelIndex &parent) {
    if(parent.isValid() || row < 0 || count < 0)
        return false;

    SceneTreeItem *parentItem = findItemBy(parent);

    //if row to start would be behind available rows (=number of children) nothing can be done
    if(parentItem->numberOfChildren() <= row)
        return false;

    //if count would be longer than children list, shorten it to the suitable size
    int lastItemToRemove = (row + count > parentItem->numberOfChildren()) ?
                                parentItem->numberOfChildren() : row + count;

    beginRemoveRows(parent, row, lastItemToRemove);
    for(int i = row; i < lastItemToRemove; ++i) {
        parentItem->removeChild(i);
    }
    endRemoveRows();

    return true;
}

SceneTreeItem* SceneTreeModel::findItemBy(const QModelIndex &index) const {
    if(index.isValid()) {
        SceneTreeItem* item = static_cast<SceneTreeItem*>(index.internalPointer());
        if(item)
            return item;
    }
    return m_rootItem;
}
