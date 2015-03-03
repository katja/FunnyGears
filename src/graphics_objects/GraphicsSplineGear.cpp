#include "graphics_objects/GraphicsSplineGear.h"
#include "preferences.h"
#include "helpers.h"
#include <glm/glm.hpp>

const int GraphicsSplineGear::Type = GraphicsSplineGear::UserType + Type::GraphicsSplineGearType;

bool GraphicsSplineGear::isGraphicsSplineGearItem(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

GraphicsSplineGear::GraphicsSplineGear(SplineGear *gear) : GraphicsSpline(gear),
    m_splineGear(gear),
    m_isBasePitchVisible(true),
    m_isReferenceCircleVisible(true),
    m_isRotating(false),
    m_rotationVelocity(0.0),
    m_rotationDegree(0.0)
{

    std::cout << "GraphicsSplineGear is created" << std::endl;

    int partColor = qrand() % 50 + 50;
    m_color = QColor(partColor / 2, partColor, (2 * partColor) % 150);

    setToolTip("Description of what will happen or what to do"); //TODO: Add description
    setFlags( QGraphicsItem::ItemIsMovable
            | QGraphicsItem::ItemIsSelectable
            | QGraphicsItem::ItemSendsGeometryChanges
            );
}

GraphicsSplineGear::~GraphicsSplineGear() {
    std::cout << "GraphicsSplineGear is deleted" << std::endl;
}

int GraphicsSplineGear::type() const {
    return Type;
}

QString GraphicsSplineGear::defaultName() const {
    return "Spline Gear";
}

real GraphicsSplineGear::module() const {
    return m_splineGear->module();
}

void GraphicsSplineGear::setModule(real module) {
    prepareGeometryChange();
    m_splineGear->setModule(module);
    changed();
}

void GraphicsSplineGear::setNumberOfTeeth(uint toothCount) {
    prepareGeometryChange();
    m_splineGear->setNumberOfTeeth(toothCount);
    changed();
}

uint GraphicsSplineGear::numberOfTeeth() const {
    return m_splineGear->numberOfTeeth();
}

void GraphicsSplineGear::setReferenceRadius(real radius) {
    prepareGeometryChange();
    m_splineGear->setReferenceRadius(radius);
    changed();
}

real GraphicsSplineGear::referenceRadius() const {
    return m_splineGear->referenceRadius();
}

real GraphicsSplineGear::minimumDistanceToCenter() const {
    if(m_splineGear->isValid())
        return m_splineGear->minimumDistanceToCenter();
    else
        return m_splineGear->minimumDistanceOfControlPointToCenter();
}

real GraphicsSplineGear::maximumDistanceToCenter() const {
    if(m_splineGear->isValid())
        return m_splineGear->maximumDistanceToCenter();
    else
        return m_splineGear->maximumDistanceOfControlPointToCenter();
}

bool GraphicsSplineGear::isBasePitchVisible() const {
    return m_isBasePitchVisible;
}

void GraphicsSplineGear::setBasePitchVisibility(bool isVisible) {
    prepareGeometryChange();
    m_isBasePitchVisible = isVisible;
}

bool GraphicsSplineGear::isReferenceCircleVisible() const {
    return m_isReferenceCircleVisible;
}

void GraphicsSplineGear::setReferenceCircleVisibility(bool isVisible) {
    prepareGeometryChange();
    m_isReferenceCircleVisible = isVisible;
}

bool GraphicsSplineGear::isRotating() const {
    return m_isRotating;
}

void GraphicsSplineGear::setRotating(bool isAnimated) {
    prepareGeometryChange();
    m_isRotating = isAnimated;
}

real GraphicsSplineGear::rotationVelocity() const {
    return m_rotationVelocity;
}

void GraphicsSplineGear::setRotationVelocity(real velocity) {
    prepareGeometryChange();
    m_rotationVelocity = velocity;
}

GearPair* GraphicsSplineGear::constructAMatingGear() const {
    return new GearPair(*m_splineGear);
}

QRectF GraphicsSplineGear::normalBoundingRect(qreal controlPointRadius) const {
    real maxRadius = Preferences::AngularPitchStrokesLength;
    if(m_splineGear != 0 && m_splineGear->isValid()) {
        real distanceToFurthestPoint = m_splineGear->maximumDistanceOfControlPointToCenter()
                                                    + controlPointRadius;
        if(distanceToFurthestPoint > maxRadius)
            maxRadius = distanceToFurthestPoint;
    }
    return QRectF(-maxRadius, -maxRadius, 2 * maxRadius, 2 * maxRadius);
}

void GraphicsSplineGear::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);

    if(m_isRotating) {
        m_rotationDegree += m_rotationVelocity;
        setRotation(m_rotationDegree);
    }

    //render line fan for better recognition of teeth:
    painter->setPen(darkenColor(m_color));
    if(m_isBasePitchVisible)
        painter->drawPath(angularPitchStrokesPath());

    //render pitch circle
    if(m_isReferenceCircleVisible)
        painter->drawPath(referenceCirclePath());

    //render spline or better, the gear:
    GraphicsSpline::paint(painter, option, widget);
}

QPainterPath GraphicsSplineGear::splineCurvePath() const {
    //TODO use only the GraphicsSpline::splineCurvePath() method ???!!!
    //     at the moment it differs only in the number of the samples
    if(!m_splineGear->isValid())
        return QPainterPath();
    // try to find out a good sampling value, which allows nice smooth curve,
    // but with as less as possible calculation time. Here the simple approach is used
    // to use 500 samples per tooth
    vector<QPointF> curve(500 * m_splineGear->numberOfTeeth());
    m_splineGear->curve(curve);
    QPainterPath path;
    path.addPolygon(QPolygonF(convertToQVector(curve)));
    return path;
}

QPainterPath GraphicsSplineGear::referenceCirclePath() const { //Wälzkreis
    return circlePath(m_splineGear->referenceRadius());
}

QPainterPath GraphicsSplineGear::angularPitchStrokesPath() const {
    vec2 startPoint(0, -Preferences::AngularPitchStrokesLength);

    if(m_splineGear->isValid()) {
        startPoint = m_splineGear->startPointForTooth();
        startPoint = glm::normalize(startPoint) * Preferences::AngularPitchStrokesLength;
    } else if(m_splineGear->numberOfControlPoints() > 0) {
        startPoint = m_splineGear->controlPoint(0);
        startPoint = glm::normalize(startPoint) * Preferences::AngularPitchStrokesLength;
    }

    QPainterPath path;
    for(uint i = 0; i < m_splineGear->numberOfTeeth(); ++i) {
        vec2 turnedPoint = m_splineGear->relatedPositionInTooth(i, startPoint);
        path.lineTo(QPointF(turnedPoint.x, turnedPoint.y));
        path.moveTo(0,0);
    }
    return path;
}
