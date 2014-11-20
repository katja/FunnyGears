#ifndef GRAPHICS_INVOLUTE_GEAR
#define GRAPHICS_INVOLUTE_GEAR

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/GraphicsScheduleItem.h"

class GraphicsInvoluteGear : public GraphicsScheduleItem {

public:
    static const int Type;// = GraphicsInvoluteGear::UserType + Type::GraphicsInvoluteGearType;
    static bool isGraphicsInvoluteGearItem(QGraphicsItem *item);


    GraphicsInvoluteGear();
    virtual ~GraphicsInvoluteGear();

    virtual QRectF boundingRect() const override; //from QGraphicsItem
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override; //from QGraphicsItem

    virtual int type() const override; //from QGraphicsItem
    virtual void receivedClickOn(QPointF point) override; //from GraphicisItem

    virtual QString defaultName() const override; //from GraphicsScheduleItem
};

#endif // GRAPHICS_INVOLUTE_GEAR
