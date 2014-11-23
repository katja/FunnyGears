#include "SplineState.h"

SplineState::~SplineState() {}

void SplineState::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
}

void SplineState::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
}

void SplineState::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
}

void SplineState::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
}

void SplineState::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
}

void SplineState::receivedClickOn(QPointF scenePos) {
    Q_UNUSED(scenePos);
}

void SplineState::paintBackground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option) {
    Q_UNUSED(pen);
    Q_UNUSED(painter);
    Q_UNUSED(option);
}

void SplineState::paintForeground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option) {
    Q_UNUSED(pen);
    Q_UNUSED(painter);
    Q_UNUSED(option);
}
