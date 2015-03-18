#include "graphics_objects/GraphicsScheduleItem.h"

const int GraphicsScheduleItem::Type = GraphicsScheduleItem::UserType + Type::GraphicsScheduleItemType;

bool GraphicsScheduleItem::isGraphicsScheduleItem(QGraphicsItem *item) {
    if(item &&
        item->type() >= Type &&
        item->type() < GraphicsScheduleItem::UserType + Type::NumberOfTypes
        )
        return true;
    return false;
}

GraphicsScheduleItem::GraphicsScheduleItem() : GraphicsItem() {
    std::cout << "GraphicsScheduleItem is created" << std::endl;
    m_name = "";
    m_parent = 0;
}

GraphicsScheduleItem::~GraphicsScheduleItem() {
    std::cout << "GraphicsScheduleItem is deleted" << std::endl;
}

int GraphicsScheduleItem::type() const {
    return Type;
}

GraphicsScheduleItem* GraphicsScheduleItem::parent() const {
    return m_parent;
}

GraphicsScheduleItem* GraphicsScheduleItem::child(int number) const {
    return m_children.value(number); // .value(n) checks the bounds of the list - .at(n) is faster but without a check!
}

int GraphicsScheduleItem::numberOfChildren() const {
    return m_children.size();
}

int GraphicsScheduleItem::childNumber() const {
    if(parent() != 0)
        return parent()->m_children.indexOf(const_cast<GraphicsScheduleItem*>(this));
    else
        return 0;
}

QString GraphicsScheduleItem::defaultName() const {
    return "Schedule Item";
}

void GraphicsScheduleItem::setName(QString name) {
    if(name != "")
        m_name = name;
}

QString GraphicsScheduleItem::name() {
    if(m_name == "")
        m_name = QString("Unnamed ") + defaultName();
    return m_name;
}

void GraphicsScheduleItem::toggleVisibility() {
    setVisible(!isVisible());
}

void GraphicsScheduleItem::clearSelection() {
    for(GraphicsScheduleItem *child : m_children) {
        child->clearSelection();
    }
    setSelected(false);
}

QVariant GraphicsScheduleItem::itemChange(GraphicsItemChange change, const QVariant &var) {
    if (change == ItemParentChange) {
        // var is the new parent
        QGraphicsItem *newParent = qvariant_cast<QGraphicsItem*>(var);
        handleNewParent(newParent);
    } else if (change == ItemChildAddedChange) {
        // var holds the child to be added
        QGraphicsItem *newChild = qvariant_cast<QGraphicsItem*>(var);
        handleChildAdded(newChild);
    } else if (change == ItemChildRemovedChange) {
        // var holds the child to be removed
        QGraphicsItem *oldChild = qvariant_cast<QGraphicsItem*>(var);
        handleChildRemoved(oldChild);
    }
    return QGraphicsItem::itemChange(change, var);
}

void GraphicsScheduleItem::handleNewParent(QGraphicsItem *newParent) {
    if(!isGraphicsScheduleItem(newParent)) {
        return;
    }
    GraphicsScheduleItem *parent = static_cast<GraphicsScheduleItem*>(newParent);
    if(m_parent == parent) {
        return;
    }
    m_parent = parent;
}

void GraphicsScheduleItem::handleChildAdded(QGraphicsItem *newChild) {
    if(!isGraphicsScheduleItem(newChild)) {
        return;
    }
    GraphicsScheduleItem *child = static_cast<GraphicsScheduleItem*>(newChild);
    if(m_children.contains(child)) {
        return;
    }
    m_children.append(child);
}

void GraphicsScheduleItem::handleChildRemoved(QGraphicsItem *oldChild) {
    if(!isGraphicsScheduleItem(oldChild)) {
        return;
    }
    GraphicsScheduleItem *child = static_cast<GraphicsScheduleItem*>(oldChild);
    m_children.removeAll(child);
}

