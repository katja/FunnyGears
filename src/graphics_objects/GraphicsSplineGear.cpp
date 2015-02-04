#include "graphics_objects/GraphicsSplineGear.h"
#include "preferences.h"
#include "helpers.h"

const int GraphicsSplineGear::Type = GraphicsSplineGear::UserType + Type::GraphicsSplineGearType;

bool GraphicsSplineGear::isGraphicsSplineGearItem(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

GraphicsSplineGear::GraphicsSplineGear(SplineGear *gear) : GraphicsSpline(gear), m_splineGear(gear) {
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

    painter->setBrush(darkenColor(m_color));
    painter->drawPath(angularPitchStrokesPath());

    GraphicsSpline::paint(painter, option, widget);
}

QPainterPath GraphicsSplineGear::splineCurvePath() const {
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


QPainterPath GraphicsSplineGear::angularPitchStrokesPath() const {
    vec2 startPoint(0, -Preferences::AngularPitchStrokesLength);

    if(m_splineGear->isValid()) {
        startPoint = m_splineGear->startPointForTooth();
        startPoint = startPoint.normalized() * Preferences::AngularPitchStrokesLength;
    } else if(m_splineGear->numberOfControlPoints() > 0) {
        startPoint = m_splineGear->controlPoint(0);
        startPoint = startPoint.normalized() * Preferences::AngularPitchStrokesLength;
    }

    QPainterPath path;
    for(uint i = 0; i < m_splineGear->numberOfTeeth(); ++i) {
        vec2 turnedPoint = m_splineGear->relatedPositionInTooth(i, startPoint);
        path.lineTo(QPointF(turnedPoint.x(), turnedPoint.y()));
        path.moveTo(0,0);
    }
    return path;
}
