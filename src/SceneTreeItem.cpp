#include "SceneTreeItem.h"

SceneTreeItem::SceneTreeItem() {
    std::cout << "SceneTreeItem-Root is created" << std::endl;
    m_isRoot = true;
}

SceneTreeItem::SceneTreeItem(QGraphicsItem *geometry, SceneTreeItem *parent) : m_geometry(geometry), m_isRoot(false), m_parent(parent) {
    std::cout << "SceneTreeItem is created" << std::endl;
    m_name = "Unnamed Item";
    m_isVisible = true;
}

SceneTreeItem::~SceneTreeItem() {
    std::cout << "SceneTreeItem is deleted" << std::endl;
    qDeleteAll(m_children);
}

bool SceneTreeItem::isRoot() const {
    return m_isRoot;
}

SceneTreeItem* SceneTreeItem::addChild(QGraphicsItem *geometry) {
    SceneTreeItem *child = new SceneTreeItem(geometry, this);
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
    m_geometry->setVisible(m_isVisible);
}

bool SceneTreeItem::isVisible() const {
    return m_isVisible;
}

bool SceneTreeItem::isRemovable() const {
    return true; //TODO: make suitable
}

QString SceneTreeItem::type() const {
    return "m_geometry"; //TODO: return suitable stuff instead of this stub!
}

QGraphicsItem* SceneTreeItem::geometry() const { //TODO: But implement the QMetaTypeClass before!!!
    return m_geometry;
}

SceneTreeItem* SceneTreeItem::itemWithGeometry(const QGraphicsItem *geometry) const {
    if(numberOfChildren() > 0) {
        foreach(SceneTreeItem *item, m_children) {
            if(item->geometry() == geometry) {
                return item;
            } else {
                SceneTreeItem *foundItem = item->itemWithGeometry(geometry);
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
