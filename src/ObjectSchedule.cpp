#include "ObjectSchedule.h"
#include <QFileSystemModel>

ObjectSchedule::ObjectSchedule(QWidget *parent) : QTreeView(parent) {

    setSelectionMode(QAbstractItemView::ExtendedSelection); //this mode allows a selection of multiple items with mouse, ctrl-key and shift-key

    // QFileSystemModel *model = new QFileSystemModel;
    // model->setRootPath(QDir::currentPath());
    sceneModel = new SceneTreeModel();
    setModel(sceneModel);

}

ObjectSchedule::~ObjectSchedule() {

}

void ObjectSchedule::addGeometry() {
    sceneModel->addGeometry();
}


//Helpermethod for removeGeometry
//TODO: put this method somewhere suitable!
bool isBelow(QModelIndex &a, QModelIndex &b) {
    if(a.row() < b.row())
        return false;
    else
        return true;
}

void ObjectSchedule::removeGeometry() {
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
        if(markedColumnsCounter >= sceneModel->columnCount()) {
            sceneModel->remove(index);
        }
    }
}
