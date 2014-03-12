#include "GraphicsScene.h"
#include "ConnectionModel.h"
#include "SceneTreeModel.h"
#include "SceneTreeItem.h"

ConnectionModel::ConnectionModel(SceneTreeModel *model) : QItemSelectionModel(model), m_sceneTreeModel(model) {
    std::cout << "ConnectionModel is created" << std::endl;
}

ConnectionModel::~ConnectionModel() {
    std::cout << "ConnectionModel is deleted" << std::endl;
}

void ConnectionModel::turnOnEditingOf(QGraphicsItem *item) {
    clearPreviousSelectionIn(ConnectionModel::ALL);
    selectItemInModel(item);
    emit startEditingItem(item);
}

void ConnectionModel::sceneSelectionChanged(GraphicsScene *scene) {
    clearPreviousSelectionIn(ConnectionModel::MODEL);
    addToModelSelection(scene);
}

void ConnectionModel::modelSelectionChanged(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) {
    if(clearingDemandedIn(command)) {
        clearPreviousSelectionIn(ConnectionModel::SCENE);
    }
    if(geometryInSelection(index, command)) {

        QGraphicsItem *graphicsItem = graphicOfRow(index);
        if(!graphicsItem)
            return;
        if(QItemSelectionModel::Select & command) {
            addToSceneSelection(graphicsItem);
        }
        if(QItemSelectionModel::Deselect & command) {
            removeFromSceneSelection(graphicsItem);
        }
    }
}

void ConnectionModel::select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) {
    modelSelectionChanged(index, command);
    QItemSelectionModel::select(index, command);
}

void ConnectionModel::select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command) {
    foreach(QModelIndex index, selection.indexes()) {
        modelSelectionChanged(index, command);
    }
    QItemSelectionModel::select(selection, command);
}

void ConnectionModel::clearPreviousSelectionIn(ConnectionModel::Connection destination) {
    if(destination == ConnectionModel::MODEL || destination == ConnectionModel::ALL)
        clearSelection(); //model: clear selection
    if(destination == ConnectionModel::SCENE || destination == ConnectionModel::ALL) { //scene: clear selection
        foreach(QGraphicsItem *item, m_selectedItems) {
            item->setSelected(false);
        }
    }
    emit updateRegionInScene(m_selectedItems); //scene: update rendering (remove highlight of previous selection)
    m_selectedItems.clear();
}

void ConnectionModel::addToModelSelection(GraphicsScene *scene) {
    m_changedItems.clear();

    QList<QGraphicsItem*> changedSceneItems = scene->selectedItems();
    foreach(QGraphicsItem *graphicsItem, changedSceneItems) {
        m_changedItems << graphicsItem;
        m_selectedItems << graphicsItem;
        selectItemInModel(graphicsItem);
    }
    emit updateRegionInScene(m_changedItems);
}

void ConnectionModel::addToSceneSelection(QGraphicsItem *graphicsItem) {
    graphicsItem->setSelected(true);
    m_changedItems.clear();
    m_changedItems << graphicsItem;
    m_selectedItems << graphicsItem;
    emit updateRegionInScene(m_changedItems);
}

void ConnectionModel::removeFromSceneSelection(QGraphicsItem *graphicsItem) {
    int index = m_selectedItems.indexOf(graphicsItem);
    if(index >= 0) { //index is in list
        graphicsItem->setSelected(false);
        m_changedItems.clear();
        m_changedItems << graphicsItem;
        m_selectedItems.removeAt(index);
        emit updateRegionInScene(m_changedItems);
    }
    if(graphicsItem->isSelected()) {
        std::cout << "ERROR: AN ITEM IS SELECTED AND MUST NOT BE!" << std::endl;
    }
}

void ConnectionModel::selectItemInModel(QGraphicsItem *item) {
    QModelIndex index = m_sceneTreeModel->itemWithGeometry(item);

    if(index.isValid()) {
        QItemSelectionModel::select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

QGraphicsItem* ConnectionModel::graphicOfRow(const QModelIndex &index) {
    QModelIndex geometryIndex = index.sibling(index.row(), SceneTreeModel::GEOM);
    if(!geometryIndex.isValid())
        return 0;
    void *data = geometryIndex.internalPointer(); //TODO: refactor SceneTreeModel and -Item not to use in every column same QModelIndex, but f.ex. in the geom column the m_geometry of the SceneTreeItem
    SceneTreeItem *item = static_cast<SceneTreeItem*>(data);
    QGraphicsItem *graphicsItem = item->geometry();
    return graphicsItem;
}

bool ConnectionModel::clearingDemandedIn(QItemSelectionModel::SelectionFlags command) {
    return QItemSelectionModel::Clear & command;
}
bool ConnectionModel::geometryInSelection(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) {
    return QItemSelectionModel::Rows & command || index.column() == SceneTreeModel::GEOM;
}
