#ifndef GRAPHICS_GEAR_PAIR
#define GRAPHICS_GEAR_PAIR

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/GraphicsScheduleItem.h"
#include "graphics_objects/GraphicsItem.h"
#include "basic_objects/GearPair.h"
#include "basic_objects/ContactPoint.h"

class GraphicsGearPair : public GraphicsScheduleItem {

public:
    static const int Type; // = GraphicsItem::UserType + Type::GraphicsGearPairType;
    static bool isGraphicsGearPair(QGraphicsItem *item);

    GraphicsGearPair(GearPair *gearPair);
    virtual ~GraphicsGearPair();

    int type() const override; // from QGraphicsItem
    QString defaultName() const override; // GraphicsItem

    void receivedClickOn(QPointF scenePos) override; //from GraphicsItem, does nothing here

    QRectF boundingRect() const override; // from QGraphicsItem
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override; // from QGraphicsItem
    void paintContactPoint(ContactPoint cp, QPainter *painter, QColor color);

private:
    GearPair *m_gearPair;

};
#endif // GRAPHICS_GEAR_PAIR
