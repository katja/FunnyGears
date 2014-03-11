#include "GraphicsScene.h"
#include "ConnectionSelectionModel.h"
#include "SceneTreeModel.h"
#include "SceneTreeItem.h"

ConnectionSelectionModel::ConnectionSelectionModel(SceneTreeModel *model) : QItemSelectionModel(model), m_sceneTreeModel(model) {
    std::cout << "ConnectionSelectionModel is created" << std::endl;
}

ConnectionSelectionModel::~ConnectionSelectionModel() {
    std::cout << "ConnectionSelectionModel is deleted" << std::endl;
}

void ConnectionSelectionModel::sceneSelectionChanged(GraphicsScene *scene) {
    clearPreviousSelectionIn(ConnectionSelectionModel::MODEL);
    addToModelSelection(scene);
}

void ConnectionSelectionModel::modelSelectionChanged(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) {
    if(clearingDemandedIn(command)) {
        clearPreviousSelectionIn(ConnectionSelectionModel::SCENE);
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

void ConnectionSelectionModel::select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) {
    modelSelectionChanged(index, command);
    QItemSelectionModel::select(index, command);
}

void ConnectionSelectionModel::select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command) {
    foreach(QModelIndex index, selection.indexes()) {
        modelSelectionChanged(index, command);
    }
    QItemSelectionModel::select(selection, command);
}

void ConnectionSelectionModel::clearPreviousSelectionIn(ConnectionSelectionModel::Connection destination) {
    if(destination == ConnectionSelectionModel::MODEL)
        clearSelection(); //model: clear selection
    if(destination == ConnectionSelectionModel::SCENE) { //scene: clear selection
        foreach(QGraphicsItem *item, m_selectedItems) {
            item->setSelected(false);
        }
    }
    emit updateRegionInScene(m_selectedItems); //scene: update rendering (remove highlight of previous selection)
    m_selectedItems.clear();
}

void ConnectionSelectionModel::addToModelSelection(GraphicsScene *scene) {
    m_changedItems.clear();

    QList<QGraphicsItem*> changedSceneItems = scene->selectedItems();
    foreach(QGraphicsItem *graphicsItem, changedSceneItems) {
        m_changedItems << graphicsItem;
        m_selectedItems << graphicsItem;
        selectItemInModel(graphicsItem);
    }
    updateRegionInScene(m_changedItems);
}

void ConnectionSelectionModel::addToSceneSelection(QGraphicsItem *graphicsItem) {
    graphicsItem->setSelected(true);
    m_changedItems.clear();
    m_changedItems << graphicsItem;
    m_selectedItems << graphicsItem;
    emit updateRegionInScene(m_changedItems);
}

void ConnectionSelectionModel::removeFromSceneSelection(QGraphicsItem *graphicsItem) {
    int index = m_selectedItems.indexOf(graphicsItem);
    if(index >= 0) { //index is in list
        graphicsItem->setSelected(false);
        m_changedItems.clear();
        m_changedItems << graphicsItem;
        m_selectedItems.removeAt(index);
        updateRegionInScene(m_changedItems);
    }
    if(graphicsItem->isSelected()) {
        std::cout << "ERROR: AN ITEM IS SELECTED AND MUST NOT BE!" << std::endl;
    }
}

void ConnectionSelectionModel::selectItemInModel(QGraphicsItem *item) {
    QModelIndex index = m_sceneTreeModel->itemWithGeometry(item);

    if(index.isValid()) {
        QItemSelectionModel::select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

QGraphicsItem* ConnectionSelectionModel::graphicOfRow(const QModelIndex &index) {
    QModelIndex geometryIndex = index.sibling(index.row(), SceneTreeModel::GEOM);
    if(!geometryIndex.isValid())
        return 0;
    void *data = geometryIndex.internalPointer(); //TODO: refactor SceneTreeModel and -Item not to use in every column same QModelIndex, but f.ex. in the geom column the m_geometry of the SceneTreeItem
    SceneTreeItem *item = static_cast<SceneTreeItem*>(data);
    QGraphicsItem *graphicsItem = item->geometry();
    return graphicsItem;
}

bool ConnectionSelectionModel::clearingDemandedIn(QItemSelectionModel::SelectionFlags command) {
    return QItemSelectionModel::Clear & command;
}
bool ConnectionSelectionModel::geometryInSelection(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) {
    return QItemSelectionModel::Rows & command || index.column() == SceneTreeModel::GEOM;
}
