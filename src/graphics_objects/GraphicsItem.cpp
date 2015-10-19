#include "graphics_objects/GraphicsItem.h"
#include "tool_bars/ToolBarManager.h"

const int GraphicsItem::Type = GraphicsItem::UserType + Type::GraphicsItemType;

bool GraphicsItem::isGraphicsItem(QGraphicsItem *item) {
    if(item &&
        item->type() >= Type &&
        item->type() < GraphicsItem::UserType + Type::NumberOfTypes
        )
        return true;
    return false;
}

GraphicsItem::GraphicsItem(GraphicsItem *parent) : QGraphicsItem(parent), m_changed(true) {
}

int GraphicsItem::type() const {
    return Type;
}

void GraphicsItem::setParentItem(QGraphicsItem *newParent) {
    ToolBarManager::getToolBarManager()->addListenerToToolBars(this);
    QGraphicsItem::setParentItem(newParent);
}

bool GraphicsItem::noItemInSceneSelected() {
    return scene()->selectedItems().empty();
}

void GraphicsItem::informAboutChange(ChangingObjectListener *listener) {
    m_changingListeners.push_back(listener);
}

void GraphicsItem::noMoreInformAboutChange(ChangingObjectListener *listener) {
    m_changingListeners.remove(listener);
}

void GraphicsItem::changed() {
    m_changed = true;
    for(ChangingObjectListener* listener : m_changingListeners) {
        listener->objectChanged(this);
    }
}

bool GraphicsItem::hasChanged() const {
    return m_changed;
}
void GraphicsItem::clearChanges() {
    m_changed = false;
}
