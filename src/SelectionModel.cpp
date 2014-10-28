#include "GraphicsScene.h"
#include "SelectionModel.h"
#include "SceneTreeModel.h"
#include "SceneTreeItem.h"

SelectionModel::SelectionModel(SceneTreeModel *model) : QItemSelectionModel(model), m_sceneTreeModel(model) {
    std::cout << "SelectionModel is created" << std::endl;
    connect(this, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(updateSelection(const QItemSelection&, const QItemSelection&)));
}

SelectionModel::~SelectionModel() {
    std::cout << "SelectionModel is deleted" << std::endl;
}

void SelectionModel::sceneSelectionChanged(GraphicsScene *scene) {
    QList<GraphicsItem*> sceneSelectionItems = scene->selectedGraphicsItems();

    foreach(GraphicsItem *currentlySelected, m_selectedItems) {
        if(!sceneSelectionItems.contains(currentlySelected)) {
            changeSelectionInModel(currentlySelected, QItemSelectionModel::Deselect);
        }
    }

    foreach(GraphicsItem* sceneItem, sceneSelectionItems) {
        if(!m_selectedItems.contains(sceneItem)) {
            changeSelectionInModel(sceneItem, QItemSelectionModel::Select);
        }
    }
    reportSelectionCount();
}

void SelectionModel::updateSelection(const QItemSelection &selected, const QItemSelection &deselected) {
    if(selectedRows(SceneTreeModel::NAME).size() == 0) {
        m_selectedItems.clear();
        emit selectNothing();
    } else if(selectedRows(SceneTreeModel::NAME).size() == 1) {
        GraphicsItem *graphicsItem = graphicOfRow(selectedRows(SceneTreeModel::NAME).first());
        if(graphicsItem) {
            m_selectedItems.clear();
            m_selectedItems << graphicsItem;
            emit selectOnly(graphicsItem);
        }
    } else {
        foreach(QModelIndex index, selected.indexes()) {
            if(index.column() == SceneTreeModel::NAME) {
                GraphicsItem *graphicsItem = graphicOfRow(index);
                if(graphicsItem) {
                    m_selectedItems << graphicsItem;
                    emit selectAlso(graphicsItem);
                }
            }
        }
        foreach(QModelIndex index, deselected.indexes()) {
            if(index.column() == SceneTreeModel::NAME) {
                GraphicsItem *graphicsItem = graphicOfRow(index);
                if(graphicsItem) {
                    m_selectedItems.removeAll(graphicsItem);
                    emit selectNoMore(graphicsItem);
                }
            }
        }
    }
    reportSelectionCount();
}

void SelectionModel::changeSelectionInModel(GraphicsItem *item, QItemSelectionModel::SelectionFlags command) {
    QModelIndex index = m_sceneTreeModel->itemWithGraphicsItem(item);

    if(index.isValid()) {
            QItemSelectionModel::select(index, command | QItemSelectionModel::Rows);
    }
}

void SelectionModel::reportSelectionCount() {
    if(m_selectedItems.size() == 1) {
        emit oneGraphicsItemSelected(m_selectedItems.first());
    } else {
        emit noneOrManyGraphicsItemsSelected();
    }
}

GraphicsItem* SelectionModel::graphicOfRow(const QModelIndex &index) {
    if(!index.isValid())
        return 0;
    void *data = index.internalPointer(); //TODO: refactor SceneTreeModel and -Item not to use in every column same QModelIndex, but f.ex. in the geom column the m_graphicsItem of the SceneTreeItem
    SceneTreeItem *item = static_cast<SceneTreeItem*>(data);
    GraphicsItem *graphicsItem = item->graphicsItem();
    return graphicsItem;
}

bool SelectionModel::selectionIncludesGraphicsItem(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) {
    return index.row() >= 0 //if nothing is selected the model uses a index of -1
        && (QItemSelectionModel::Rows & command);
}
