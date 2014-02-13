#include "SceneTreeItem.h"

SceneTreeItem::SceneTreeItem() {
    m_isRoot = true;
}

SceneTreeItem::SceneTreeItem(Geometry *geometry, SceneTreeItem *parent) : m_geometry(geometry), m_parent(parent), m_isRoot(false) {
}

SceneTreeItem::~SceneTreeItem() {
    qDeleteAll(m_children);
}

bool SceneTreeItem::isRoot() const {
    return m_isRoot;
}

SceneTreeItem* SceneTreeItem::addChild(Geometry *geometry) {
    SceneTreeItem *child = new SceneTreeItem(geometry, this);
    m_children.append(child);
    return child;
}

bool SceneTreeItem::removeChild(SceneTreeItem *child) {
    int childIndex = m_children.indexOf(child);
    if(childIndex < 0) {
        return false; //child not found
    } else if(child->isRemovable()) {
        m_children.removeAt(childIndex);
        return true;
    } else {
        return false; //child is not removable
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
}

bool SceneTreeItem::isVisible() const {
    return m_isVisible;
}

bool SceneTreeItem::isRemovable() const {
    return true; //TODO: make suitable
}
