#ifndef GRAPHICS_SPLINE_GEAR
#define GRAPHICS_SPLINE_GEAR

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/GraphicsSpline.h"
#include "basic_objects/SplineGear.h"


class GraphicsSplineGear : public GraphicsSpline {

public:
    static const int Type; // = GraphicsSplineGear::UserType + Type::GraphicsSplineGearType;
    static bool isGraphicsSplineGearItem(QGraphicsItem *item);

    GraphicsSplineGear();
    virtual ~GraphicsSplineGear();

    int type() const override; // from QGraphicsItem
    virtual QString defaultName() const override; // from GraphicsScheduleItem

    QRectF boundingRect() const override; // from QGraphicsItem
    // void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override; // from QGraphicsItem

    void receivedClickOn(QPointF scenePos) override; // from GraphicsItem


private:
    SplineGear *m_splineGear;
    // real m_radius;
    // uint m_numberOfTeeth;

    QColor m_color;

    QPainterPath angularPitchStrokesPath();
};

#endif // GRAPHICS_SPLINE_GEAR
