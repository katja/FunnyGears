#ifndef GRAPHICS_MATING_SPLINE_GEAR
#define GRAPHICS_MATING_SPLINE_GEAR

#include "stable.h"
#include "graphics_objects/GraphicsSplineGear.h"
class GraphicsGearPair;

class GraphicsMatingSplineGear : public GraphicsSplineGear {

public:
    GraphicsMatingSplineGear(SplineGear *gear, GraphicsGearPair *parent);
    ~GraphicsMatingSplineGear();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override; // from QGraphicsItem

private:
    GraphicsGearPair *m_graphicsGearPair;
};

#endif // GRAPHICS_MATING_SPLINE_GEAR
