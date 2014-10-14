#ifndef GRAPHICS_SIMPLE_GEAR
#define GRAPHICS_SIMPLE_GEAR

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/GraphicsItem.h"
#include "basic_objects/SimpleGear.h"


class GraphicsSimpleGear : public GraphicsItem {

public:
    static const int Type;// = GraphicsSimpleGear::UserType + Type::GraphicsSplineType;
    static bool isGraphicsSimpleGearItem(QGraphicsItem *item);

    GraphicsSimpleGear(GraphicsItem *parent = 0);
    // GraphicsSimpleGear(ToothProfile *toothProfile, ToothProfileGraphicsItem *parent = 0);
    virtual ~GraphicsSimpleGear();

    // GraphicsSimpleGear* copy() const;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    int type() const;
    void clickReceived(QPointF point, Editing::State state);
    QString defaultName() const;


private:
    SimpleGear *m_simpleGear;
    real m_radius;
    uint m_numberOfTeeth;

    QColor m_color;

    QPainterPath angularPitchStrokesPath();
};

#endif // GRAPHICS_SIMPLE_GEAR
