#include "PenSplineState.h"

PenSplineState::PenSplineState(GraphicsSpline *spline) : m_spline(spline) {}
PenSplineState::~PenSplineState() {}

QRectF PenSplineState::boundingRect() const {
    return m_spline->normalBoundingRect();
}

QPainterPath PenSplineState::shape() const {
    if(m_spline->noItemInSceneSelected())
        return m_spline->normalShape();
    else
        return QPainterPath();
}

void PenSplineState::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    m_spline->update();
}

void PenSplineState::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    m_spline->update();
}

void PenSplineState::receivedClickOn(QPointF scenePos) {
    m_spline->addPoint(scenePos);
    m_spline->update();
}

void PenSplineState::paintBackground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option) {

    if(option->state & QStyle::State_Sunken)
        pen.setColor(Preferences::ActiveColor);
    else if(option->state & QStyle::State_Selected)
        pen.setColor(Preferences::SelectionColor);
    else if(option->state & QStyle::State_MouseOver && m_spline->noItemInSceneSelected())
        pen.setColor(Preferences::HoverColor);
    else
        return;

    painter->setPen(pen);
    painter->drawPath(m_spline->controlPointPolygonPath());
    painter->drawPath(m_spline->controlPointsPaths());
    painter->drawPath(m_spline->splineCurvePath());
}
