#ifndef GRAPHICS_ITEM
#define GRAPHICS_ITEM

#include "stable.h"
#include "definitions.h"
#include "EditingStatesAndActions.h"

class GraphicsItem : public QGraphicsItem {

public:
    static const int Type;// = GraphicsItem::UserType + Type::GraphicsItemType;
    static bool isGraphicsItem(QGraphicsItem *item);

    GraphicsItem(GraphicsItem *parent = 0);
    virtual ~GraphicsItem() {}

    virtual int type() const;
    virtual void clickReceived(QPointF point, Editing::State state) = 0;
    virtual void setToState(Editing::State state) = 0;

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

    QList<GraphicsItem*> childItems() const;
    QList<GraphicsItem*> collidingItems(Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const;
    GraphicsItem* commonAncestorItem(const GraphicsItem *other) const;
    GraphicsItem* focusItem() const;
    GraphicsItem* focusProxy() const;
    GraphicsItem* panel() const;
    GraphicsItem* parentItem() const;
    GraphicsItem* topLevelItem() const;

private:
    QList<GraphicsItem*> toGraphicsList(QList<QGraphicsItem*> qlist) const;
};

#endif // GRAPHICS_ITEM
