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

    GraphicsSplineGear(SplineGear *gear);
    virtual ~GraphicsSplineGear();

    int type() const override; // from QGraphicsItem
    virtual QString defaultName() const override; // from GraphicsScheduleItem

    QRectF normalBoundingRect(qreal controlPointRadius = Preferences::PointRadius + 0.5f * Preferences::HighlightedLineWidth) const override; // from GraphicsSpline
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override; // from QGraphicsItem


private:
    SplineGear *m_splineGear;

    QColor m_color;

    QPainterPath splineCurvePath() const override; // from GraphicsSpline
    QPainterPath angularPitchStrokesPath() const;
};

#endif // GRAPHICS_SPLINE_GEAR
