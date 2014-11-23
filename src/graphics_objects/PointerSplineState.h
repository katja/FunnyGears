#ifndef POINTER_SPLINE_STATE
#define POINTER_SPLINE_STATE
#include "graphics_objects/SplineState.h"
#include "graphics_objects/GraphicsSpline.h"

class PointerSplineState : public SplineState {

public:
    PointerSplineState(GraphicsSpline *spline);
    ~PointerSplineState();

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverUpdate(QGraphicsSceneHoverEvent *event);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void paintBackground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option);

private:
    GraphicsSpline *m_spline;

    bool m_mouseIsOverPoint;
    bool m_mouseIsOverEdge;
    int m_indexOfPointUnderMouse;
    QPointF m_lastMousePos;

    qreal m_snappingAreaSize;
};
#endif // POINTER_SPLINE_STATE
