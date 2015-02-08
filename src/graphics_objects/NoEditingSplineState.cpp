#include "NoEditingSplineState.h"

NoEditingSplineState::NoEditingSplineState(GraphicsSpline *spline) : m_spline(spline) {}

NoEditingSplineState::~NoEditingSplineState() {}

QRectF NoEditingSplineState::boundingRect() const {
    return m_spline->normalBoundingRect();
}

QPainterPath NoEditingSplineState::shape() const {
    return m_spline->normalShape();
}

void NoEditingSplineState::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    m_spline->update();
}

void NoEditingSplineState::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    m_spline->update();
}

void NoEditingSplineState::paintBackground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option) {
    if(option->state & QStyle::State_MouseOver)
        pen.setColor(Preferences::HoverColor);
    else if(option->state & QStyle::State_Selected)
        pen.setColor(Preferences::SelectionColor);
    else
        return;

    painter->setPen(pen);
    if(m_spline->isControlPolygonVisible()) {
        painter->drawPath(m_spline->controlPolygonPath());
        painter->drawPath(m_spline->controlPointsPaths());
    }
    painter->drawPath(m_spline->splineCurvePath());
}
