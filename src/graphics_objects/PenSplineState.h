#ifndef PEN_SPLINE_STATE
#define PEN_SPLINE_STATE

#include "graphics_objects/GraphicsSpline.h"

class PenSplineState : public SplineState {

public:
    PenSplineState(GraphicsSpline *spline);
    ~PenSplineState();

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    void receivedClickOn(QPointF scenePos);

    void paintBackground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option);

private:
    GraphicsSpline *m_spline;
};
#endif // PEN_SPLINE_STATE
