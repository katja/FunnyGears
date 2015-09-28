#include "main_gui_objects/GraphicsScene.h"
#include "main_gui_objects/SelectionModel.h"
#include "main_gui_objects/Model.h"

SelectionModel::SelectionModel(Model *model) :
    QItemSelectionModel(model),
    m_model(model),
    m_connectionsAreActivated(true)
{
    connect(this, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(updateSelection(const QItemSelection&, const QItemSelection&)));
    connect(m_model, SIGNAL(rowsAboutToBeRemoved()),
        this, SLOT(pauseSelectionConnection()));
    connect(m_model, SIGNAL(rowsFinishedRemoval()),
        this, SLOT(continueSelectionConnection()));
}

SelectionModel::~SelectionModel() {
}

void SelectionModel::sceneSelectionChanged(GraphicsScene *scene) {
    if(!m_connectionsAreActivated)
        return;
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
    if(!m_connectionsAreActivated)
        return;
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

void SelectionModel::pauseSelectionConnection() {
    m_connectionsAreActivated = false;
}
void SelectionModel::continueSelectionConnection() {
    m_connectionsAreActivated = true;
    m_selectedItems.clear();
    updateSelection(selection(), QItemSelection());
}

void SelectionModel::changeSelectionInModel(GraphicsScheduleItem *item, QItemSelectionModel::SelectionFlags command) {
    if(!m_connectionsAreActivated)
        return;
    QModelIndex index = m_model->getIndexFromItem(item);

    if(index.isValid()) {
        QItemSelectionModel::select(index, command | QItemSelectionModel::Rows);
    }
}

void SelectionModel::reportSelectionCount() {
    if(!m_connectionsAreActivated)
        return;
    if(m_selectedItems.size() == 1) {
        emit oneGraphicsItemSelected(m_selectedItems.first());
    } else {
        emit noneOrManyGraphicsItemsSelected();
    }
}
