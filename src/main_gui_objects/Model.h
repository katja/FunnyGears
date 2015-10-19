#ifndef MODEL
#define MODEL

#include "stable.h"
#include "graphics_objects/GraphicsScheduleItem.h"
#include "helper_objects/ChangingObjectResponder.h"

/** Model (tree) of the ModelTreeView and the whole scene
 *  Responsible for adding, organising and removing items
 *
 *  Holds a tree of all GraphicsScheduleItems – the root of the tree is an GraphicsRootItem
 *  and is created in the initializer.
 *  This root item is added to the QGraphicsScene scene, too, so all other items of the
 *  model are added as children of this root item. It is not visible, as it has no graphic
 *  representation.
 *  As all GraphicsScheduleItems (the root item included) are installed on the scene,
 *  the scene is responsible for the deletion of these items.)
 *
 *  To be able to use the ModelTreeView in the ObjectScheduleViewWidget it uses Qt's
 *  QAbstractItemModel. Therefore it has implementations of the methods index(), parent(),
 *  hasChildren(), rowCount(), columnCount(), data(), setData(), headerData() and flags().
 *
 *  Qt uses objects of QModelIndex to represent the data in a QAbstractItemModel. The Model
 *  used here has a fixed number of columns (described in enum DATA). The number of rows
 *  complies with the number of children of the root item and may grow and shrink. In every
 *  cell of one row is saved the same internal pointer – the pointer to the corresponding
 *  GraphicsScheduleItem. It only differs when data() is called. Depending on the column
 *  data() calls the name() or the isVisible() on the GraphicsScheduleItem. It is no good
 *  design to have these same pointers saved several times in each row. But this approach
 *  makes the connection to the tree view easier.
 *
 *  Items can only be added as top level items! But the GraphicsScheduleItems can have
 *  children on their own (by calling setParentItem(...) on the children). Access to the
 *  child items it available by the model, too. Thus the top level items are responsible
 *  for the creation and the deletion of their child items!!!
 *
 *  When items are deleted it is very essential to inform other components of the removal.
 *  Qt provides and demandes therefore the beginRemoveRows() and the endRemoveRows() methods.
 *  As Model provides methods to remove many items at once but internal handles this by
 *  removing item by item, two new signals are introduced by class Model.
 *  QAbstractItemModel::beginRemoveRows(const QModelIndex &parent, int first, int last)
 *  emits the signal QAbstractItemModel::rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
 *  Inspired by this the signal rowsAboutToBeRemoved() is emitted before any item is removed
 *  (and also before the beginRemoveRows(...) is called, therefore even before the first
 *  QAbstractItemModel::rowsAboutToBeRemoved(...) signal is emitted)
 *  And after all the removal has finished (and also the last endRemoveRows() had been called)
 *  signal rowsFinishedRemoval() is emitted.
 */
class Model : public QAbstractItemModel, public ChangingObjectResponder {

Q_OBJECT

public:
    /** Constructor of a Model for the given @param scene
     *  Note that the model uses a special invisible root item, which is added to the scene
     *  As this root item is added first here in the constructor, any QGraphicsItems (or
     *  GraphicsScheduleItems) added before, are ignored!
     */
    Model(QGraphicsScene *scene, QObject *parent = 0);
    virtual ~Model();

    /** Describes the different columns available in this model
     */
    enum Data {
        NAME, VISIBILITY
    };

    /** Returns the index of the item in the model
     *  specified by the given row, column and parent index.
     */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const; //When reimplementing this function in a subclass, call createIndex() to generate model indexes that other components can use to refer to items in your model.

    /** Returns the parent of the model item with the given index.
     *  If the item has no parent, an invalid QModelIndex is returned.
     *
     *  A common convention is that only the items in the first column have children.
     *  Therefore the column of the returned QModelIndex would always be 0.
     */
    QModelIndex parent(const QModelIndex &index) const; //When reimplementing this function in a subclass, be careful to avoid calling QModelIndex member functions, such as QModelIndex::parent(), since indexes belonging to your model will simply call your implementation, leading to infinite recursion.

    /** Returns true, if the corresponding GraphicsScheduleItem is found and has any
     *  children; otherwise returns false.
     */
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    /** Returns the number of rows under the given parent.
     *  When the parent is valid it means that rowCount is returning
     *  the number of children of parent.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /** Returns the number of columns for the children of the given parent.
     * In this implementation the number of columns is independent of the given parent
     * and always 2.
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

    /** Toggles the boolean values (up to now there is only the visibility value).
     *  Returns true, if toggling was possible.
     */
    bool toggleValue(const QModelIndex &index);

    /** Returns the data for the given role and section in the header
     *  with the specified orientation.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /** Returns the item flags for the given index.
     */
    Qt::ItemFlags flags(const QModelIndex &index) const; //The base class implementation returns a combination of flags that enables the item (ItemIsEnabled) and allows it to be selected (ItemIsSelectable). ItemIsEditable must be returned!

    /** Adds given newItem to model as top level item
     */
    bool addItem(GraphicsScheduleItem *newItem);

    /** @brief Removes the single row item with QModelIndex index
     *
     *  The row to which index belongs, is removed. Only top level items are removed by
     *  the Model! When index is not a top level item, false is returned.
     *  To inform other components about the removal and the completion of the removal,
     *  the signals rowsAboutToBeRemoved() and rowsFinishedRemoval are emitted.
     *  @return true if the row of the index could properly be removed
     */
    bool remove(QModelIndex index);

    /** @brief Removes all top level rows, which the given QModelIndexList indices belong to
     *
     *  In fact it only deletes the rows, if an item of indices in column 0 is given.
     *  A QModelIndex with another column is not handles at all. But it is expected that
     *  all items of a row are listed in indices, as they all belong to the row.
     *  All QModelIndexes which are not top level items, that means they have another parent
     *  than the root item, are not deleted and should be deleted by their parents themselves!
     *
     *  To prevent other components of trying to access items, while the removal is in
     *  progress, the signal rowsAboutToBeRemoved() is emitted before the first item is
     *  removed. The signal rowsFinishedRemoval is emitted when the whole removal is
     *  finished.
     *  @return true if every row given by the indices could be properly removed or when
     *               no row is given by indices, false otherwise
     */
    bool remove(QModelIndexList indices);

    /** @brief Removes all items of the Model except the root item
     *
     *  To prevent other components of trying to access items, while the removal is in
     *  progress, the signal rowsAboutToBeRemoved() is emitted before the first item is
     *  removed. The signal rowsFinishedRemoval is emitted when the whole removal is
     *  finished.
     */
    void removeAll();

    /** Clears the selection of every item in the model
     */
    void clearSelection();

    /** Returns a pointer to the GraphicsScheduleItem, which belongs to the given QModelIndex.
     *  If Index is not valid or no corresponding GraphicsScheduleItem can be found
     *  nullptr is returned.
     */
    GraphicsScheduleItem* getItemFromIndex(const QModelIndex &index) const;

    /** Returns the QModelIndex (in column 0) which belongs to the given GraphicsScheduleItem.
     */
    QModelIndex getIndexFromItem(GraphicsScheduleItem *item) const;

    bool hasChanged() const override; // from ChangingObjectResponder
    void clearChanges() override; // from ChangingObjectResponder

signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    // void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void rowsAboutToBeRemoved();
    void rowsFinishedRemoval();

private:
    QGraphicsScene *m_scene;
    GraphicsScheduleItem *m_rootItem;
    QHash< Data, QHash<Qt::ItemDataRole, QString> > m_headHash;
    bool m_changed;

    /** @brief Removes the given (top level) row
     *
     *  This method does not emit necessary signals to inform other components of the removal!
     *  It acts only as a helper method for the public methods to remove items! Do not use
     *  it in another way!
     */
    bool removeRow(int row);

    /** @brief Removes all dependencies and deletes item
     */
    void deleteItem(GraphicsScheduleItem *item);

    /** @brief Informs all current ChangingObjectListener to this object about changes
     */
    // void changed();

    bool haveItemsChanged() const;

    void clearChangedInItems();


    /** @brief Like @see changed() but also emits dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>& = QVector<int> ()))
     */
    void dataChanged(const QModelIndex &index);

    /** Returns a pointer to the GraphicsScheduleItem, which belongs to the given QModelIndex.
     *  If Index is not valid or no corresponding GraphicsScheduleItem can be found
     *  m_rootItem is returned.
     */
    GraphicsScheduleItem* getInternItemFromIndex(const QModelIndex &index) const;
};

#endif // MODEL
