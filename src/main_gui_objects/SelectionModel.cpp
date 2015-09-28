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
        this, SLOT(prepareRowRemoval()));
    connect(m_model, SIGNAL(rowsFinishedRemoval()),
        this, SLOT(continueAfterRowRemoval()));
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

// Maybe it seems like it would be easier to use the selection() method, here.
// But as every no longer selected GraphicsScheduleItem needs to be deselected, this would
// result in first deselcting all selected items and then again selecting the current
// selected ones. So this would mean more selection changes of the GraphicsScheduleItems
// On the other hand, implemented like at the moment, there is the cost to search for
// special items in the QLists, which is not negligible
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

void SelectionModel::prepareRowRemoval() {
    // Important to first pause all selections. Otherwise deselection would result in a bad access!
    pauseSelectionConnection();
    // Deselect all selected items to prevent inaccurate access
    deselectAll();
}

void SelectionModel::continueAfterRowRemoval() {
    //assure, that no selection of the GraphicsScheduleItems is active and m_selectedItems is empty
    deselectAll();
    continueSelectionConnection();
    updateSelection(selection(), QItemSelection());
}
void SelectionModel::pauseSelectionConnection() {
    m_connectionsAreActivated = false;
}

void SelectionModel::continueSelectionConnection() {
    m_connectionsAreActivated = true;
}

void SelectionModel::deselectAll() {
    for(GraphicsScheduleItem *item : m_selectedItems) {
        item->setSelected(false);
    }
    m_selectedItems.clear();
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
    for(QModelIndex index : selection().indexes()) {
        if(index.parent() == QModelIndex()) {
            emit topLevelItemsAreSelected();
            return;
        }
    }
    emit noTopLevelItemsAreSelected();
}
