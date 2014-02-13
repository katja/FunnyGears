#include "SceneTreeModel.h"

SceneTreeModel::SceneTreeModel(QObject *parent) : QAbstractItemModel(parent) {

    headerDataStrings.append(new QVariant("Visibility"));
    visibilityColumn = 0;
    headerDataStrings.append(new QVariant("Name"));
    nameColumn = 1;

    modelListNames.append(new QString("blub 1"));
    modelListVisibilities.append(true);
    modelListNames.append(new QString("blub 2"));
    modelListVisibilities.append(true);
    modelListNames.append(new QString("blub 3"));
    modelListVisibilities.append(true);

    // emit layoutChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);
    emit dataChanged(QModelIndex(), QModelIndex());
}

SceneTreeModel::~SceneTreeModel() {
    qDeleteAll(modelListNames);
    qDeleteAll(headerDataStrings);
}

QModelIndex SceneTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if(row < 0 || row > modelListNames.size())
        return QModelIndex();
    if(parent.isValid())
        //TODO: do something usefull here - up to now only list is used, no tree, therefore QModelIndex as return value
        return QModelIndex();
    if(column == visibilityColumn)
        return createIndex(row, column, (modelListVisibilities.at(row)));
    else
        return createIndex(row, column, modelListNames.at(row));
}

/** Returns th parent of the model item with the given index.
 *  If the item has no parent, an invalid QModelIndex is returned.
 *
 *  A common convention is that only the items in the first column have children.
 *  Therefore the column of the returned QModelIndex would always be 0.
 */
QModelIndex SceneTreeModel::parent(const QModelIndex &index) const {
    return QModelIndex(); //TODO
} //When reimplementing this function in a subclass, be careful to avoid calling QModelIndex member functions, such as QModelIndex::parent(), since indexes belonging to your model will simply call your implementation, leading to infinite recursion.

//Not necessary, but if implementation of rowCount is expensive, it would be good:
/** Returns true if parent has any children; otherwise returns false.
 */
bool SceneTreeModel::hasChildren(const QModelIndex &parent) const {
    // std::cout << "SceneTreeModel::hasChildren called" << std::endl;
    if(parent.isValid())
        return false;
    else
        return true; //TODO
}

/** Returns the number of rows under the given parent.
 *  When the parent is valid it means that rowCount is returning
 *  the number of children of parent.
 */
int SceneTreeModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid())
        return 0;
    else
        return modelListNames.size();
    //TODO!!!
}

int SceneTreeModel::columnCount(const QModelIndex &parent) const {
    return headerDataStrings.size();
}

QVariant SceneTreeModel::data(const QModelIndex &index, int role) const {
    if(index.isValid() && role == Qt::DisplayRole) {
        if(index.column() == nameColumn)
            return QVariant(*(modelListNames.at(index.row())));
        if(index.column() == visibilityColumn)
            return QVariant(modelListVisibilities.at(index.row()));
    }
    return QVariant();
}

bool SceneTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role != Qt::EditRole)
        return false;
    if(index.column() == visibilityColumn) {
        // toggleVisibility(index); TODO
    } else if (index.column() == nameColumn) {
        if(static_cast<QString*>(index.internalPointer()) == modelListNames.at(index.row())) {
            modelListNames[index.row()] = new QString(value.toString());
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
    if(section < 0 || section >= headerDataStrings.size())
        return QVariant();
    if(role == Qt::ToolTipRole){
        return *(headerDataStrings[section]); //TODO: use something suitable here!
    } else if(role == Qt::DisplayRole) {
        return *(headerDataStrings[section]);
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

bool SceneTreeModel::addGeometry() {
    int lastRow = rowCount(QModelIndex());
    return insertRows(lastRow, 1, QModelIndex());
}

bool SceneTreeModel::remove(QModelIndex index) {
    if(!index.isValid() || hasChildren(index))
        return false;
    return removeRows(index.row(), 1, index.parent());
}

bool SceneTreeModel::insertRows(int row, int count, const QModelIndex &parent) {
    if(parent.isValid() || row > modelListNames.size() || row < 0)
        return false;
    beginInsertRows(parent, row, count);
    for(int i = 0; i < count; ++i) {
        modelListNames.insert(row, new QString("Unnamed Geometry"));
        modelListVisibilities.insert(row, true);
    }
    endInsertRows();
    return true;
}

bool SceneTreeModel::removeRows(int row, int count, const QModelIndex &parent) {
    if(parent.isValid() || row > modelListNames.size() || row < 0 || count < 0)
        return false;

    //if count would be longer than children list, shorten it to the suitable size
    int lastItemToRemove = (row + count > rowCount(parent)) ? rowCount(parent): row + count;

    beginRemoveRows(parent, row, lastItemToRemove);
    for(int i = row; i < lastItemToRemove; ++i) {

        //if children are available, delete them too:
        QModelIndex indexToRemove = index(i, 0, parent); //TODO: is the "0" for column here correct???!!!
        if(hasChildren(indexToRemove)) {
            removeRows(0, rowCount(indexToRemove), indexToRemove);
        }
        //delete row:
        delete modelListNames.at(i);
        modelListNames.removeAt(i);
        modelListVisibilities.removeAt(i);
    }
    endRemoveRows();

    return true;
}
