#include "GraphicsItem.h"

const int GraphicsItem::Type = GraphicsItem::UserType + Type::GraphicsItemType;

bool GraphicsItem::isGraphicsItem(QGraphicsItem *item) {
    if(item->type() >= Type && item->type() < GraphicsItem::UserType + Type::NumberOfTypes)
        return true;
    return false;
}

GraphicsItem::GraphicsItem(GraphicsItem *parent) : QGraphicsItem(parent) {
}

int GraphicsItem::type() const {
    return Type;
}

//TODO: delete this method!!! This is only for testing and understanding of information of qt
QVariant GraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    // std::cout << "GraphicsItem::itemChange(" << change << ", " << value.toString().toStdString() << ") called" << std::endl;
    return QGraphicsItem::itemChange(change, value);
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
