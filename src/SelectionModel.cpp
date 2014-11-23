#include "GraphicsScene.h"
#include "SelectionModel.h"
#include "Model.h"

SelectionModel::SelectionModel(Model *model) : QItemSelectionModel(model), m_model(model) {
    std::cout << "SelectionModel is created" << std::endl;
    connect(this, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(updateSelection(const QItemSelection&, const QItemSelection&)));
}

SelectionModel::~SelectionModel() {
    std::cout << "SelectionModel is deleted" << std::endl;
}

void SelectionModel::sceneSelectionChanged(GraphicsScene *scene) {
    QList<QGraphicsItem*> selectedItems = scene->selectedItems();
    //Deselection:
    foreach(GraphicsScheduleItem *currentlySelected, m_selectedItems) {
        if(!selectedItems.contains(currentlySelected)) {
            changeSelectionInModel(currentlySelected, QItemSelectionModel::Deselect);
        }
    }
    //Selection:
    foreach(QGraphicsItem* sceneItem, selectedItems) {
        if(GraphicsScheduleItem::isGraphicsScheduleItem(sceneItem)) {
            GraphicsScheduleItem *scheduleItem = static_cast<GraphicsScheduleItem*>(sceneItem);
            if(!m_selectedItems.contains(scheduleItem)) {
                changeSelectionInModel(scheduleItem, QItemSelectionModel::Select);
            }
        }
    }
    reportSelectionCount();
}

void SelectionModel::updateSelection(const QItemSelection &selected, const QItemSelection &deselected) {
    foreach(QModelIndex index, selected.indexes()) {
        if(index.isValid() && index.column() == Model::NAME) {
            GraphicsScheduleItem *graphicsItem = m_model->getItemFromIndex(index);
            if(graphicsItem) {
                graphicsItem->setSelected(true);
                m_selectedItems << graphicsItem;
            }
        }
    }
    foreach(QModelIndex index, deselected.indexes()) {
        if(index.isValid() && index.column() == Model::NAME) {
            GraphicsScheduleItem *graphicsItem = m_model->getItemFromIndex(index);
            if(graphicsItem) {
                graphicsItem->setSelected(false);
                m_selectedItems.removeAll(graphicsItem);
            }
        }
    }
    reportSelectionCount();
}

void SelectionModel::changeSelectionInModel(GraphicsScheduleItem *item, QItemSelectionModel::SelectionFlags command) {
    QModelIndex index = m_model->getIndexFromItem(item);

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
