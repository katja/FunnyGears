#ifndef SCENE_TREE_MODEL
#define SCENE_TREE_MODEL

#include "stable.h"
#include "SceneTreeItem.h"

class SceneTreeModel : public QAbstractItemModel {

Q_OBJECT

public:
    SceneTreeModel(QObject *parent = 0);
    ~SceneTreeModel();

    /** Sets a new root item. Root Item may be an item of quite another scene tree.
     *  Return value is the old root item.
     *  You have to delete it, if it isn't needed any longer!
     */
    SceneTreeItem* setRoot(SceneTreeItem *rootItem);

    /** Returns the index of the item in the model
     * specified by the given row, column and parent index.
     */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const; //When reimplementing this function in a subclass, call createIndex() to generate model indexes that other components can use to refer to items in your model.

    /** Returns the parent of the model item with the given index.
     *  If the item has no parent, an invalid QModelIndex is returned.
     *
     *  A common convention is that only the items in the first column have children.
     *  Therefore the column of the returned QModelIndex would always be 0.
     */
    QModelIndex parent(const QModelIndex &index) const; //When reimplementing this function in a subclass, be careful to avoid calling QModelIndex member functions, such as QModelIndex::parent(), since indexes belonging to your model will simply call your implementation, leading to infinite recursion.

    //Not necessary, but if implementation of rowCount is expensive, it would be good:
    /** Returns true if parent has any children; otherwise returns false.
     */
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    /** Returns the number of rows under the given parent.
     *  When the parent is valid it means that rowCount is returning
     *  the number of children of parent.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /** Returns the number of columns for the children of the given parent.
      * In this implementation the number of columns is independent of the given parent
      * and always 2. TODO: still correct?
      */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;


    /** Returns the data stored under the given role for the item referred to by the index.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const; //If you do not have a value to return, return an invalid QVariant instead of returning 0.

    /** Sets the role data for the item at index to value.
     *  Returns true if successful; otherwise returns false.
     *  dataChanged() signal is emitted, if data was successfully set
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole); //emit dataChanged() signal

    /** Returns the data for the given role and section in the header
     *  with the specified orientation.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /** Returns the item flags for the given index.
     */
    Qt::ItemFlags flags(const QModelIndex &index) const; //The base class implementation returns a combination of flags that enables the item (ItemIsEnabled) and allows it to be selected (ItemIsSelectable). ItemIsEditable must be returned!

    bool addItem();
    bool remove(QModelIndex index);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

signals:
    void layoutChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
    SceneTreeItem* findItemBy(const QModelIndex &index) const;

    SceneTreeItem   *m_rootItem;
    QList<QVariant*> m_headerData;
    int m_nameColumn;
    int m_visibilityColumn;
};

#endif // SCENE_TREE_MODEL
