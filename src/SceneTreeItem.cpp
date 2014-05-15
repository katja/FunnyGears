#include "SceneTreeItem.h"

SceneTreeItem::SceneTreeItem(GraphicsScene *graphicsScene) : m_graphicsScene(graphicsScene) {
    std::cout << "SceneTreeItem-Root is created" << std::endl;
    m_isRoot = true;
}

SceneTreeItem::SceneTreeItem(GraphicsItem *graphicsItem, SceneTreeItem *parent) : m_isRoot(false), m_parent(parent), m_graphicsItem(graphicsItem) {
    std::cout << "SceneTreeItem is created" << std::endl;
    m_name = "Unnamed Item";
    m_isVisible = true;
    m_graphicsScene = m_parent->m_graphicsScene;
    m_graphicsScene->addItem(graphicsItem);
}

SceneTreeItem::~SceneTreeItem() {
    std::cout << "SceneTreeItem is deleted" << std::endl;
    qDeleteAll(m_children);
    delete m_graphicsItem;
}

bool SceneTreeItem::isRoot() const {
    return m_isRoot;
}

SceneTreeItem* SceneTreeItem::addChild(GraphicsItem *graphicsItem) {
    SceneTreeItem *child = new SceneTreeItem(graphicsItem, this);
    foreach(GraphicsItem *graphicsChild, graphicsItem->childItems()) {
        child->addChild(graphicsChild);
    }
    m_children.append(child);
    return child;
}

bool SceneTreeItem::removeChild(SceneTreeItem *child) {
    int childIndex = m_children.indexOf(child);
    return removeChild(childIndex);
}

bool SceneTreeItem::removeChild(int childIndex) {
    if(childIndex < 0 || childIndex >= numberOfChildren() || !child(childIndex)->isRemovable()) {
        return false; //child not found/non-existent/not removable
    } else {
        //children of child(childIndex) are deleted by Qt by deleting child(childIndex)
        delete child(childIndex);
        m_children.removeAt(childIndex);
        return true;
    }
}

SceneTreeItem* SceneTreeItem::parent() const {
    return m_parent;
}

SceneTreeItem* SceneTreeItem::child(int number) const {
    return m_children.value(number); // .value(n) checks the bounds of the list - .at(n) is faster but without a check!
}

int SceneTreeItem::numberOfChildren() const {
    return m_children.size();
}

int SceneTreeItem::childNumber() const {
    if(!isRoot())
        return parent()->m_children.indexOf(const_cast<SceneTreeItem*>(this));
    else
        return 0;
}

void SceneTreeItem::setName(QString name) {
    m_name = name;
}

QString SceneTreeItem::name() const {
    return m_name;
}

void SceneTreeItem::toggleVisibility() {
    m_isVisible = !m_isVisible;
    m_graphicsItem->setVisible(m_isVisible);
}

bool SceneTreeItem::isVisible() const {
    return m_isVisible;
}

bool SceneTreeItem::isRemovable() const {
    return m_parent->isRoot();
}

QString SceneTreeItem::type() const {
    return "m_graphicsItem"; //TODO: return suitable stuff instead of this stub!
}

GraphicsItem* SceneTreeItem::graphicsItem() const { //TODO: But implement the QMetaTypeClass before!!!
    return m_graphicsItem;
}

SceneTreeItem* SceneTreeItem::itemWithGraphicsItem(const GraphicsItem *graphicsItem) const {
    if(numberOfChildren() > 0) {
        foreach(SceneTreeItem *item, m_children) {
            if(item->graphicsItem() == graphicsItem) {
                return item;
            } else {
                SceneTreeItem *foundItem = item->itemWithGraphicsItem(graphicsItem);
                if(foundItem) {
                    return foundItem;
                }
            }
        }
    }
    return 0;
}

void SceneTreeItem::translate(const QVector2D &translation) {
    m_translation += translation;
}

QVector2D SceneTreeItem::getTranslation() const {
    return m_translation;
}

void SceneTreeItem::rotate(float angle) {
    m_rotation += angle;
}

float SceneTreeItem::getRotation() const {
    return m_rotation;
}
