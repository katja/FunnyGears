#ifndef GRAPHICS_SIMPLE_GEAR
#define GRAPHICS_SIMPLE_GEAR

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/GraphicsScheduleItem.h"
#include "basic_objects/SimpleGear.h"


class GraphicsSimpleGear : public GraphicsScheduleItem {

public:
    static const int Type; // = GraphicsSimpleGear::UserType + Type::GraphicsSimpleGearType;
    static bool isGraphicsSimpleGearItem(QGraphicsItem *item);

    GraphicsSimpleGear();
    virtual ~GraphicsSimpleGear();

    int type() const override; // from QGraphicsItem
    virtual QString defaultName() const override; // from GraphicsScheduleItem

    QRectF boundingRect() const override; // from QGraphicsItem
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override; // from QGraphicsItem

    void receivedClickOn(QPointF scenePos) override; // from GraphicsItem


private:
    SimpleGear *m_simpleGear;
    real m_radius;
    uint m_numberOfTeeth;

    QColor m_color;

    QPainterPath angularPitchStrokesPath();
};

#endif // GRAPHICS_SIMPLE_GEAR
