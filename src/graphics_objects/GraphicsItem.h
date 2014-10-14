#ifndef GRAPHICS_ITEM
#define GRAPHICS_ITEM

#include "stable.h"
#include "definitions.h"
#include "tool_bars/EditingStatesAndActions.h"

class GraphicsItem : public QGraphicsItem {

public:
    static const int Type;// = GraphicsItem::UserType + Type::GraphicsItemType;
    static bool isGraphicsItem(QGraphicsItem *item);

    GraphicsItem(GraphicsItem *parent = 0);
    virtual ~GraphicsItem() {}

    virtual int type() const;
    virtual void clickReceived(QPointF point, Editing::State state) = 0;

    virtual QString defaultName() const;
    QString name() const;
    void rename(const QString &name);

    QList<GraphicsItem*> childItems() const;
    QList<GraphicsItem*> collidingItems(Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const;
    GraphicsItem* commonAncestorItem(const GraphicsItem *other) const;
    GraphicsItem* focusItem() const;
    GraphicsItem* focusProxy() const;
    GraphicsItem* panel() const;
    GraphicsItem* parentItem() const;
    GraphicsItem* topLevelItem() const;

protected:
    QString m_name;

private:
    QList<GraphicsItem*> toGraphicsList(QList<QGraphicsItem*> qlist) const;
};

#endif // GRAPHICS_ITEM
