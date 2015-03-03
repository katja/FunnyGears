#include "GraphicsMatingSplineGear.h"
#include "graphics_objects/GraphicsGearPair.h"

GraphicsMatingSplineGear::GraphicsMatingSplineGear(SplineGear *gear, GraphicsGearPair *parent) :
    GraphicsSplineGear(gear),
    m_graphicsGearPair(parent)
{
    std::cout << "GraphicsMatingSplineGear is created" << std::endl;
    setParentItem(m_graphicsGearPair);

    disableUserInteraction();
}

GraphicsMatingSplineGear::~GraphicsMatingSplineGear() {
    std::cout << "GraphicsMatingSplineGear is deleted" << std::endl;
}

void GraphicsMatingSplineGear::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    GraphicsSplineGear::paint(painter, option, widget);
    m_graphicsGearPair->paintAdditionals(painter, this);
}

void GraphicsMatingSplineGear::disableUserInteraction() {
    setFlags(0x0);
    setAcceptHoverEvents(false);
    m_isUserInteractionEnabled = false;
}

void GraphicsMatingSplineGear::enableUserInteraction() {
    setFlags(QGraphicsItem::ItemIsSelectable
            | QGraphicsItem::ItemSendsGeometryChanges
            );
    setAcceptHoverEvents(true);
    m_isUserInteractionEnabled = true;
}

bool GraphicsMatingSplineGear::isUserInteractionEnabled() const {
    return m_isUserInteractionEnabled;
}

QRectF GraphicsMatingSplineGear::normalBoundingRect(qreal controlPointRadius) const {
    if(m_isUserInteractionEnabled)
        return GraphicsSplineGear::normalBoundingRect(controlPointRadius);
    return QRectF();
    // real maxRadius = Preferences::AngularPitchStrokesLength;
    // if(m_splineGear != 0 && m_splineGear->isValid()) {
    //     real distanceToFurthestPoint = m_splineGear->maximumDistanceOfControlPointToCenter()
    //                                                 + controlPointRadius;
    //     if(distanceToFurthestPoint > maxRadius)
    //         maxRadius = distanceToFurthestPoint;
    // }
    // return QRectF(-maxRadius, -maxRadius, 2 * maxRadius, 2 * maxRadius);
}

QPainterPath GraphicsMatingSplineGear::normalShape() const {
    if(m_isUserInteractionEnabled)
        return GraphicsSplineGear::normalShape();
    return QPainterPath();
}

void GraphicsMatingSplineGear::startEditing(Editing::State editingState) {
    if(m_isUserInteractionEnabled)
        GraphicsSplineGear::startEditing(editingState);
}

void GraphicsMatingSplineGear::stopEditing() {
    if(m_isUserInteractionEnabled)
        GraphicsSplineGear::stopEditing();
}
