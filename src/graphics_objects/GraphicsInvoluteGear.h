#ifndef GRAPHICS_INVOLUTE_GEAR
#define GRAPHICS_INVOLUTE_GEAR

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/GraphicsItem.h"

class GraphicsInvoluteGear : public GraphicsItem {

public:
    static const int Type;// = GraphicsItem::UserType + Type::GraphicsInvoluteGearType;
    static bool isGraphicsInvoluteGearItem(QGraphicsItem *item);


    GraphicsInvoluteGear(GraphicsItem *parent = 0);
    virtual ~GraphicsInvoluteGear();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual int type() const;
    virtual void clickReceived(QPointF point, Editing::State state);

    virtual QString defaultName() const;
};

#endif // GRAPHICS_INVOLUTE_GEAR
