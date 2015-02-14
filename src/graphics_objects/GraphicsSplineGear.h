#ifndef GRAPHICS_SPLINE_GEAR
#define GRAPHICS_SPLINE_GEAR

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/GraphicsSpline.h"
#include "basic_objects/SplineGear.h"
#include "basic_objects/GearPair.h"


class GraphicsSplineGear : public GraphicsSpline {

public:
    static const int Type; // = GraphicsSplineGear::UserType + Type::GraphicsSplineGearType;
    static bool isGraphicsSplineGearItem(QGraphicsItem *item);

    GraphicsSplineGear(SplineGear *gear);
    virtual ~GraphicsSplineGear();

    int type() const override; // from QGraphicsItem
    virtual QString defaultName() const override; // from GraphicsScheduleItem

    real module() const;
    void setModule(real module);

    void setNumberOfTeeth(uint toothCount);
    uint numberOfTeeth() const;

    void setReferenceRadius(real radius);
    real referenceRadius() const;

    real minimumDistanceToCenter() const;
    real maximumDistanceToCenter() const;

    bool isBasePitchVisible() const;
    void setBasePitchVisibility(bool isVisible);

    bool isReferenceCircleVisible() const;
    void setReferenceCircleVisibility(bool isVisible);

    bool isRotating() const;
    void setRotating(bool);
    real rotationVelocity() const;
    void setRotationVelocity(real velocity);

    GearPair* constructAMatingGear() const;

    QRectF normalBoundingRect(qreal controlPointRadius = Preferences::PointRadius + 0.5 * Preferences::HighlightedLineWidth) const override; // from GraphicsSpline
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override; // from QGraphicsItem


private:
    SplineGear *m_splineGear;

    bool m_isBasePitchVisible;
    bool m_isReferenceCircleVisible;

    bool m_isRotating;
    real m_rotationVelocity;
    real m_rotationDegree;

    /** @brief Creates the path of the gear
     *  @see GraphicsSpline::splineCurvePath()
     */
    QPainterPath splineCurvePath() const override; // from GraphicsSpline

    QPainterPath referenceCirclePath() const;

    /** @brief Creates a fan-shaped path which marks the pitches of the gear
     *
     *  If a curve can be layed through the control points, the lines pass the start
     *  point of the visible curve of each tooth (not the first control point).
     *  If the relation of degree and controlpoints does not allow a rendering
     *  of the curve, the lines pass the first control point of the tooth profile.
     *  If no control point is available, one line of the fan will go from center on
     *  upwards.
     *  @return fan-shaped path to mark the teeth or better said the pitches
     */
    QPainterPath angularPitchStrokesPath() const;
};

#endif // GRAPHICS_SPLINE_GEAR
