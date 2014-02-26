#include "ObjectSchedule.h"
#include "YesNoDelegate.h"

ObjectSchedule::ObjectSchedule(QWidget *parent) : QTreeView(parent) {

    setHeaderHidden(false);
    setSelectionMode(QAbstractItemView::ExtendedSelection); //this mode allows a selection of multiple items with mouse, ctrl-key and shift-key
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::DoubleClicked);

    setItemDelegate(new YesNoDelegate());

    // QFileSystemModel *model = new QFileSystemModel;
    // model->setRootPath(QDir::currentPath());
    m_sceneModel = new SceneTreeModel();
    setModel(m_sceneModel);

    hideColumn(SceneTreeModel::TRANSLATION);
    hideColumn(SceneTreeModel::ROTATION);
    m_numberOfHiddenColumns = 2;
}

ObjectSchedule::~ObjectSchedule() {
    delete m_sceneModel;
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

