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

    void disableUserInteraction();
    void enableUserInteraction();
    bool isUserInteractionEnabled() const;

    QRectF normalBoundingRect(qreal controlPointRadius) const override; // from GraphicsSplineGear
    QPainterPath normalShape() const override; // from GraphicsSpline

    void startEditing(Editing::State editingState) override; // from GraphicsSpline
    void stopEditing() override; // from GraphicsSpline

private:
    GraphicsGearPair *m_graphicsGearPair;
    bool m_isUserInteractionEnabled;
};

#endif // GRAPHICS_MATING_SPLINE_GEAR
