#include "EraserSplineState.h"

EraserSplineState::EraserSplineState(GraphicsSpline *spline) :
    m_spline(spline),
    m_indexOfPointUnderMouse(-1),
    m_snappingAreaSize(Preferences::PointRadius + Preferences::HighlightedLineWidth)
{
}

EraserSplineState::~EraserSplineState() {}

QRectF EraserSplineState::boundingRect() const {
    return m_spline->normalBoundingRect(m_snappingAreaSize);
}

QPainterPath EraserSplineState::shape() const {
    return m_spline->controlPointsPaths(m_snappingAreaSize);
}

void EraserSplineState::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    m_indexOfPointUnderMouse = m_spline->indexOfPointAt(event->pos(), m_snappingAreaSize);
    m_spline->update();
}

void EraserSplineState::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    m_indexOfPointUnderMouse = -1;
    m_spline->update();
}

void EraserSplineState::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    if(m_indexOfPointUnderMouse < 0) {
        m_indexOfPointUnderMouse = m_spline->indexOfPointAt(event->pos(), m_snappingAreaSize);
        m_spline->update();
    }
}

void EraserSplineState::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(m_indexOfPointUnderMouse < 0)
        return;

    // scenePos is in circle around controlpoint c,
    // if (x - c_x)^2 + (y - c_y)^2 <= radius^2
    QPointF p = event->pos();
    QPointF c = m_spline->positionOfPoint(m_indexOfPointUnderMouse);

    if((p.x() - c.x()) * (p.x() - c.x()) + (p.y() - c.y()) * (p.y() - c.y()) <= m_snappingAreaSize * m_snappingAreaSize)
        m_spline->removePoint(m_indexOfPointUnderMouse);
    m_indexOfPointUnderMouse = -1;
    event->accept();
}

void EraserSplineState::paintBackground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option) {
    if(option->state & QStyle::State_Selected) {
        if(option->state & QStyle::State_Sunken)
            pen.setColor(Preferences::ActiveColor);
        else if(option->state & QStyle::State_Selected)
            pen.setColor(Preferences::SelectionColor);

        painter->setPen(pen);
        painter->drawPath(m_spline->controlPointPolygonPath());
        painter->drawPath(m_spline->controlPointsPaths());
        painter->drawPath(m_spline->splineCurvePath());
    }

    if(m_indexOfPointUnderMouse >= 0) {
        pen.setColor(Preferences::HoverColor);
        painter->setPen(pen);
        painter->drawEllipse(m_spline->positionOfPoint(m_indexOfPointUnderMouse), Preferences::PointRadius, Preferences::PointRadius);
    }
}

void EraserSplineState::paintForeground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option) {
    Q_UNUSED(option);
    if(m_indexOfPointUnderMouse >= 0) {
        pen.setColor(Preferences::AttentionColor);
        painter->setPen(pen);
        QPointF length = QPointF(m_snappingAreaSize, m_snappingAreaSize);
        QPointF start = m_spline->positionOfPoint(m_indexOfPointUnderMouse) - 0.5f * length;
        painter->drawLine(start, start + length);
    }
    pen.setWidth(0);
    painter->setPen(pen);
    painter->drawPath(m_spline->controlPointsPaths(m_snappingAreaSize));
}
