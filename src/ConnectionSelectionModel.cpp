#include "GraphicsScene.h"
#include "ConnectionSelectionModel.h"
#include "SceneTreeModel.h"

ConnectionSelectionModel::ConnectionSelectionModel(SceneTreeModel *model) : QItemSelectionModel(model), m_sceneTreeModel(model) {
    std::cout << "ConnectionSelectionModel is created" << std::endl;
}

ConnectionSelectionModel::~ConnectionSelectionModel() {
    std::cout << "ConnectionSelectionModel is deleted" << std::endl;
}

void ConnectionSelectionModel::sceneSelectionChanged(GraphicsScene *scene) {
    clearSelection();

    QList<QGraphicsItem*> changedSceneItems = scene->selectedItems();
    foreach(QGraphicsItem *graphicsItem, changedSceneItems) {
        QModelIndex foundItem = m_sceneTreeModel->itemWithGeometry(graphicsItem);

        if(foundItem.isValid()) {
            QModelIndex startOfRow = foundItem.sibling(foundItem.row(), 0);
            QModelIndex endOfRow = foundItem.sibling(foundItem.row(), m_sceneTreeModel->columnCount() - 1);
            select(QItemSelection(startOfRow, endOfRow), QItemSelectionModel::Select);
        }
    }
}
