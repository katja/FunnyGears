#ifndef GRAPHICS_ITEM
#define GRAPHICS_ITEM

#include "stable.h"
#include "definitions.h"

class GraphicsItem : public QGraphicsItem {

public:
    static const int Type;
    static bool isGraphicsItem(QGraphicsItem *item);

    GraphicsItem(GraphicsItem *parent = 0);
    virtual ~GraphicsItem() {}

    virtual int type() const;
    virtual void clickReceived(QPointF point) = 0;
    virtual void setToState(EditingState state) = 0;

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
