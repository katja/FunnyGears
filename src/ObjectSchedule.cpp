#include "ObjectSchedule.h"
#include <QFileSystemModel>

ObjectSchedule::ObjectSchedule(QWidget *parent) : QTreeView(parent) {

    setSelectionMode(QAbstractItemView::ExtendedSelection); //this mode allows a selection of multiple items with mouse, ctrl-key and shift-key
    setHeaderHidden(false);

    // QFileSystemModel *model = new QFileSystemModel;
    // model->setRootPath(QDir::currentPath());
    m_sceneModel = new SceneTreeModel();
    setModel(m_sceneModel);

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
        if(markedColumnsCounter >= m_sceneModel->columnCount()) {
            m_sceneModel->remove(index);
        }
    }
}
