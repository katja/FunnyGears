#include "graphics_objects/GraphicsMatingSplineGear.h"
#include "graphics_objects/GraphicsGearPair.h"

GraphicsMatingSplineGear::GraphicsMatingSplineGear(SplineGear *gear, GraphicsGearPair *parent) :
    GraphicsSplineGear(gear),
    m_graphicsGearPair(parent)
{
    setParentItem(m_graphicsGearPair);
    disableUserInteraction();
}

GraphicsMatingSplineGear::~GraphicsMatingSplineGear() {
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
    return m_graphicsGearPair->boundingRect(this);
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
