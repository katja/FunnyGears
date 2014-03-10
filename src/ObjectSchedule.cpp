#include "ObjectSchedule.h"
#include "YesNoDelegate.h"

ObjectSchedule::ObjectSchedule(SceneTreeModel *sceneModel, QWidget *parent) : QTreeView(parent), m_sceneModel(sceneModel) {

    std::cout << "ObjectSchedule is created" << std::endl;

    setHeaderHidden(false);
    setSelectionMode(QAbstractItemView::ExtendedSelection); //this mode allows a selection of multiple items with mouse, ctrl-key and shift-key
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::DoubleClicked);

    setItemDelegate(new YesNoDelegate(this));

    setModel(m_sceneModel);

    hideColumn(SceneTreeModel::TRANSLATION);
    hideColumn(SceneTreeModel::ROTATION);
    hideColumn(SceneTreeModel::GEOM);
    m_numberOfHiddenColumns = 3;
}

ObjectSchedule::~ObjectSchedule() {
    std::cout << "ObjectSchedule is deleted" << std::endl;
    // delete m_sceneModel;
}

void ObjectSchedule::addItem() {
    m_sceneModel->addItem();
}


//Helpermethod for removeItems()
//TODO: put this method somewhere suitable!
bool isBelow(QModelIndex &a, QModelIndex &b) {
    if(a.row() < b.row())
        return false;
    else
        return true;
}

void ObjectSchedule::removeItems() {
    QModelIndexList selectedItems = selectedIndexes();

    //Warn the user with a message box:
    if(selectedItems.empty())
        return;

    QString messageText = "Do you really want to delete the selected ";
    if(selectedItems.size() > 1)
        messageText.append("items?");
    else
        messageText.append("item?");

    QMessageBox askPermissionDialog(this);
    askPermissionDialog.setText(messageText);
    askPermissionDialog.setInformativeText("Selected items will have gone until doomsday! You can't undo this action!");
    askPermissionDialog.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    askPermissionDialog.setDefaultButton(QMessageBox::Yes);
    int answer = askPermissionDialog.exec();
    if(answer == QMessageBox::Yes) { // = 1 => dialog accepted => delete items

        //sort items from bottom to top, otherwise row number will be useless after first rows are deleted
        qSort(selectedItems.begin(), selectedItems.end(), isBelow);

        //only delete entries, whose line (row) was marked fully!
        foreach(QModelIndex index, selectedItems) {
            int markedColumnsCounter = 1;
            foreach(QModelIndex index2, selectedItems) {
                if(index.row() == index2.row() && index.column() < index2.column()) {
                    ++markedColumnsCounter;
                }
            }
            if(markedColumnsCounter >= m_sceneModel->columnCount() - m_numberOfHiddenColumns) {
                m_sceneModel->remove(index);
            }
        }
    }
}
