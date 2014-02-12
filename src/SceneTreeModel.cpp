#include "SceneTreeModel.h"

SceneTreeModel::SceneTreeModel(QObject *parent) : QAbstractItemModel(parent) {
    // std::cout << "SceneTreeModel will be created" << std::endl;

    modelList = new QList<QString*>();
    modelList->append(new QString("blub 1"));
    modelList->append(new QString("blub 2"));
    modelList->append(new QString("blub 3"));
    // std::cout << "ModelList created with "<< modelList->size() << " entries" << std::endl;

    // emit layoutChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);
    emit dataChanged(QModelIndex(), QModelIndex());
}

SceneTreeModel::~SceneTreeModel() {

}
/** Returns the index of the item in the model
 * specified by the given row, column and parent index.
 */
QModelIndex SceneTreeModel::index(int row, int column, const QModelIndex &parent) const {
    // std::cout << "SceneTreeModel::index called" << std::endl;
    if(parent.isValid() || row < 0 || row > modelList->size())
        return QModelIndex();
    else
        return createIndex(row, column, modelList->at(row)); //TODO: add a last argument with: void *ptr

} //When reimplementing this function in a subclass, call createIndex() to generate model indexes that other components can use to refer to items in your model.

/** Returns the parent of the model item with the given index.
 *  If the item has no parent, an invalid QModelIndex is returned.
 *
 *  A common convention is that only the items in the first column have children.
 *  Therefore the column of the returned QModelIndex would always be 0.
 */
QModelIndex SceneTreeModel::parent(const QModelIndex &index) const {
    // std::cout << "SceneTreeModel::parent called" << std::endl;
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
    // std::cout << "SceneTreeModel::rowCount called" << std::endl;
    if(parent.isValid())
        return 0;
    else
        return modelList->size();
    //TODO!!!
}

/** Returns the number of columns for the children of the given parent.
  * In this implementation the number of columns is independent of the given parent
  * and always 2. TODO: still correct?
  */
int SceneTreeModel::columnCount(const QModelIndex &parent) const {
    // std::cout << "SceneTreeModel::columnCount called" << std::endl;
    return 2; //TODO
}


/** Returns the data stored under the given role for the item referred to by the index.
 */
QVariant SceneTreeModel::data(const QModelIndex &index, int role) const {
    // std::cout << "SceneTreeModel::data is asked" << std::endl;
    if(index.isValid() && role == Qt::DisplayRole)
        return QVariant(*(modelList->at(index.row())));
    return QVariant(); //TODO
} //If you do not have a value to return, return an invalid QVariant instead of returning 0.

/** Sets the role data for the item at index to value.
 *  Returns true if successful; otherwise returns false.
 *  dataChanged() signal is emitted, if data was successfully set
 */
bool SceneTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    // std::cout << "SceneTreeModel::setData called" << std::endl;
    return false; //TODO
} //emit dataChanged() signal

/** Returns the item flags for the given index.
 */
Qt::ItemFlags SceneTreeModel::flags(const QModelIndex &index) const {
    // std::cout << "SceneTreeModel::flags called" << std::endl;

    if(index.isValid()) {
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    } else {
        return Qt::NoItemFlags;
    }
} //The base class implementation returns a combination of flags that enables the item (ItemIsEnabled) and allows it to be selected (ItemIsSelectable). ItemIsEditable must be returned!

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
    if(parent.isValid() || row > modelList->size() || row < 0)
        return false;
    beginInsertRows(parent, row, count);
    for(int i = 0; i < count; ++i)
        modelList->insert(row, new QString("Unnamed Geometry"));
    endInsertRows();
    return true;
}

bool SceneTreeModel::removeRows(int row, int count, const QModelIndex &parent) {
    if(parent.isValid() || row > modelList->size() || row < 0 || count < 0)
        return false;

    //if count would be longer than children list, shorten it to the suitable size
    int lastItemToRemove = (row + count > rowCount(parent)) ? rowCount(parent): row + count;
    QModelIndex indexToRemove = index(row, 0, parent); //TODO: is the "0" for column here correct???!!!

    //if children are available, delete them too.
    if(hasChildren(indexToRemove)) {
        removeRows(0, rowCount(indexToRemove), indexToRemove);
    }

    //delete the rows:
    beginRemoveRows(parent, row, lastItemToRemove);
    for(int i = row; i < lastItemToRemove; ++i) {
        modelList->removeAt(i); //TODO: what happens with the strings???
    }
    endRemoveRows();

    return true;
}
