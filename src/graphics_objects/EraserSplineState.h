#ifndef ERASER_SPLINE_STATE
#define ERASER_SPLINE_STATE

#include "graphics_objects/SplineState.h"
#include "graphics_objects/GraphicsSpline.h"

class EraserSplineState : public SplineState {

public:
    EraserSplineState(GraphicsSpline *spline);
    ~EraserSplineState();

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void paintBackground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option);
    void paintForeground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option);

private:
    GraphicsSpline *m_spline;

    int m_indexOfPointUnderMouse;
    qreal m_snappingAreaSize;
};

#endif // ERASER_SPLINE_STATE
