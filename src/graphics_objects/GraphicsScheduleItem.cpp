#include "GraphicsScheduleItem.h"

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

//TODO: isn't it better to use a setVisibility() here???
void GraphicsScheduleItem::toggleVisibility() {
    setVisible(!isVisible());
}

QVariant GraphicsScheduleItem::itemChange(GraphicsItemChange change, const QVariant &var) {
    std::cout << "    itemChange " << change << ": ";
    if (change == ItemParentChange) {
        std::cout << "        ItemParentHasChanged" << std::endl;
        // var is the new parent
        QGraphicsItem *newParent = qvariant_cast<QGraphicsItem*>(var);
        handleNewParent(newParent);
    } else if (change == ItemChildAddedChange) {
        std::cout << "        ItemChildAddedChange" << std::endl;
        // var holds the child to be added
        QGraphicsItem *newChild = qvariant_cast<QGraphicsItem*>(var);
        handleChildAdded(newChild);
    } else if (change == ItemChildRemovedChange) {
        std::cout << "        ItemChildRemovedChange" << std::endl;
        // var holds the child to be removed
        QGraphicsItem *oldChild = qvariant_cast<QGraphicsItem*>(var);
        handleChildRemoved(oldChild);
    } else if (change == ItemSceneChange) {
        std::cout << "        ItemSceneChange" << std::endl;
        // var is the new scene, scene() has still the old one
        QGraphicsScene *scene = qvariant_cast<QGraphicsScene*>(var);
        handleSceneChange(scene);
    // } else if (change == ItemSelectedHasChanged) {
    //     std::cout << "        ItemSelectedHasChanged" << std::endl;
    //     // var is the new isSelected boolean (true/false)
    //     bool value = var.toBool();
    //     handleSelectionChange(value);
    } else {
        std::cout << std::endl;
    }
    return QGraphicsItem::itemChange(change, var);
}

void GraphicsScheduleItem::handleNewParent(QGraphicsItem *newParent) {
    std::cout << "handleNewParent ";
    if(!isGraphicsScheduleItem(newParent)) {
        std::cout << "found NO suitable item!!!" << std::endl;
        return;
    }
    GraphicsScheduleItem *parent = static_cast<GraphicsScheduleItem*>(newParent);
    if(m_parent == parent) {
        std::cout << "found already has this parent!!!" << std::endl;
        return;
    }
    m_parent = parent;
    std::cout << "finished" << std::endl;
}

void GraphicsScheduleItem::handleChildAdded(QGraphicsItem *newChild) {
    std::cout << "handleChildAdded ";
    if(!isGraphicsScheduleItem(newChild)) {
        std::cout << "found NO suitable child!!!" << std::endl;
        return;
    }
    GraphicsScheduleItem *child = static_cast<GraphicsScheduleItem*>(newChild);
    if(m_children.contains(child)) {
        std::cout << "found this child already in its child items!!!" << std::endl;
        return;
    }
    m_children.append(child);
    std::cout << "finished" << std::endl;
}

void GraphicsScheduleItem::handleChildRemoved(QGraphicsItem *oldChild) {
    std::cout << "handleChildRemoved ";
    if(!isGraphicsScheduleItem(oldChild)) {
        std::cout << "found NO suitable item to remove!!!" << std::endl;
        return;
    }
    GraphicsScheduleItem *child = static_cast<GraphicsScheduleItem*>(oldChild);
    int countRemoved = m_children.removeAll(child);
    std::cout << "removed " << countRemoved << " children, finished" << std::endl;
}

// TODO: if this method isn't used, delete it!!!
void GraphicsScheduleItem::handleSceneChange(QGraphicsScene* newScene) {
    std::cout << "handleSceneChange finished" << std::endl;
}

// void GraphicsScheduleItem::handleSelectionChange(bool isSelected) {
//     std::cout << "handleSelectionChange finished" << std::endl;
// }
