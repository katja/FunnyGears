#include "graphics_objects/GraphicsItem.h"

const int GraphicsItem::Type = GraphicsItem::UserType + Type::GraphicsItemType;

bool GraphicsItem::isGraphicsItem(QGraphicsItem *item) {
    if(item->type() >= Type && item->type() < GraphicsItem::UserType + Type::NumberOfTypes)
        return true;
    return false;
}

GraphicsItem::GraphicsItem(GraphicsItem *parent) : QGraphicsItem(parent), m_name("") {
}

int GraphicsItem::type() const {
    return Type;
}

QString GraphicsItem::defaultName() const {
    return "Item";
}

QString GraphicsItem::name() const {
    if(m_name == "")
        return defaultName();
    return m_name;
}
void GraphicsItem::rename(const QString &name) {
    m_name = name;
}

QList<GraphicsItem*> GraphicsItem::childItems() const {
    return toGraphicsList(QGraphicsItem::childItems());
}

QList<GraphicsItem*> GraphicsItem::collidingItems(Qt::ItemSelectionMode mode) const {
    return toGraphicsList(QGraphicsItem::collidingItems(mode));
}

GraphicsItem* GraphicsItem::commonAncestorItem(const GraphicsItem *other) const {
    GraphicsItem *graphicsItem = qgraphicsitem_cast<GraphicsItem*>(QGraphicsItem::commonAncestorItem(other));
    assert(graphicsItem != 0);
    return graphicsItem;
}

GraphicsItem* GraphicsItem::focusItem() const {
    GraphicsItem *graphicsItem = qgraphicsitem_cast<GraphicsItem*>(QGraphicsItem::focusItem());
    assert(graphicsItem != 0);
    return graphicsItem;
}

GraphicsItem* GraphicsItem::focusProxy() const {
    GraphicsItem *graphicsItem = qgraphicsitem_cast<GraphicsItem*>(QGraphicsItem::focusProxy());
    assert(graphicsItem != 0);
    return graphicsItem;
}

GraphicsItem* GraphicsItem::parentItem() const {
    GraphicsItem *graphicsItem = qgraphicsitem_cast<GraphicsItem*>(parentItem());
    assert(graphicsItem != 0);
    return graphicsItem;
}

GraphicsItem* GraphicsItem::topLevelItem() const {
    GraphicsItem *graphicsItem = qgraphicsitem_cast<GraphicsItem*>(topLevelItem());
    assert(graphicsItem != 0);
    return graphicsItem;

}

QList<GraphicsItem*> GraphicsItem::toGraphicsList(QList<QGraphicsItem*> qlist) const {
    QList<GraphicsItem*> list;
    for(int i = 0; i < qlist.size(); ++i) {
        list << qgraphicsitem_cast<GraphicsItem*>(qlist.at(i));
        assert(list.last() != 0);
    }
    return list;
}
