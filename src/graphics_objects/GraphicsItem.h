#ifndef GRAPHICS_ITEM
#define GRAPHICS_ITEM

#include "stable.h"
#include "definitions.h"
#include "tool_bars/EditingToolBarListener.h"

class GraphicsItem : public QGraphicsItem, public EditingToolBarListener {

public:
    static const int Type;// = GraphicsItem::UserType + Type::GraphicsItemType;
    static bool isGraphicsItem(QGraphicsItem *item);

    GraphicsItem(GraphicsItem *parent = 0);
    virtual ~GraphicsItem() {}

    virtual int type() const override;
    virtual void receivedClickOn(QPointF point) = 0;

    void setParentItem(QGraphicsItem *newParent); //override from QGraphicsItem

    virtual void stopEditing() override {}
    virtual void startEditing(Editing::State) override {};
    virtual void executeEditingAction(Editing::Action) override {};

};

#endif // GRAPHICS_ITEM
