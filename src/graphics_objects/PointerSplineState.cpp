#include "helpers.h"
#include "graphics_objects/PointerSplineState.h"

PointerSplineState::PointerSplineState(GraphicsSpline *spline) :
    m_spline(spline),
    m_mouseIsOverPoint(false),
    m_mouseIsOverEdge(false),
    m_indexOfPointUnderMouse(-1),
    m_lastMousePos(QPointF(0, 0)),
    m_snappingAreaSize(Preferences::PointRadius + Preferences::HighlightedLineWidth) {
}

PointerSplineState::~PointerSplineState() {}

QRectF PointerSplineState::boundingRect() const {
    return m_spline->normalBoundingRect(m_snappingAreaSize);
}

QPainterPath PointerSplineState::shape() const {
    if(m_mouseIsOverPoint)
        return m_spline->controlPointsPaths(m_snappingAreaSize);
    else
        return m_spline->normalShape();
}

void PointerSplineState::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    hoverUpdate(event);
}

void PointerSplineState::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    m_indexOfPointUnderMouse = -1;
    m_lastMousePos = QPointF(0, 0);
    m_mouseIsOverEdge = false;
    m_mouseIsOverPoint = false;
    m_spline->update();
}

void PointerSplineState::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    if(m_mouseIsOverPoint) {
        // mouse pointer is still in circle around controlpoint c, if
        // (x - c_x)^2 + (y - c_y)^2 <= radius^2
        qreal x = event->pos().x();
        qreal y = event->pos().y();
        QPointF c = m_spline->positionOfPoint(m_indexOfPointUnderMouse);

        if(((x - c.x()) * (x - c.x()) + (y - c.y()) * (y - c.y()) > m_snappingAreaSize * m_snappingAreaSize)) {
            m_mouseIsOverPoint = false;
            hoverUpdate(event);
        }
    } else { // mouse is over edge
        // As the hoverUpdate() may be an expensive calculation, especially with many
        // control points, try to do it not so often. Therefore it is only tested again
        // when mouse has moved more than the manhatten length of m_snappingAreaSize
        if((m_lastMousePos - event->pos()).manhattanLength() > m_snappingAreaSize) {
            m_mouseIsOverEdge = false; // This may be incorrect. But is is necessary for correct result of hoverUpdate().
            hoverUpdate(event);
        }
    }
}

void PointerSplineState::hoverUpdate(QGraphicsSceneHoverEvent *event) {
    m_indexOfPointUnderMouse = m_spline->indexOfPointAt(event->pos(), m_snappingAreaSize);
    if(m_indexOfPointUnderMouse >= 0) {
        m_mouseIsOverPoint = true;
    } else {
        m_mouseIsOverEdge = true;
        m_lastMousePos = event->pos();
    }
    m_spline->update();
}

void PointerSplineState::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(m_mouseIsOverPoint) {
        m_spline->mouseMovePointEvent(m_indexOfPointUnderMouse, event);
    } else {
        m_spline->mouseMoveWholeSplineEvent(event);
    }
}

void PointerSplineState::paintBackground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option) {
    if(m_mouseIsOverEdge || option->state & QStyle::State_Selected) {
        if(m_mouseIsOverEdge)
            pen.setColor(Preferences::HoverColor);
        else if(option->state & QStyle::State_Selected)
            pen.setColor(Preferences::SelectionColor);

        painter->setPen(pen);
        if(m_spline->isControlPolygonVisible()) {
            painter->drawPath(m_spline->controlPolygonPath());
            painter->drawPath(m_spline->controlPointsPaths());
        }
        painter->drawPath(m_spline->splineCurvePath());
    }

    if(m_mouseIsOverPoint) {
        pen.setColor(Preferences::HoverColor);
        painter->setPen(pen);
        painter->drawEllipse(m_spline->positionOfPoint(m_indexOfPointUnderMouse), Preferences::PointRadius, Preferences::PointRadius);
    }
}
