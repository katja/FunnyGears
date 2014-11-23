#ifndef NO_EDITING_SPLINE_STATE
#define NO_EDITING_SPLINE_STATE
#include "graphics_objects/GraphicsSpline.h"

class NoEditingSplineState : public SplineState {

public:
    NoEditingSplineState(GraphicsSpline *spline);
    ~NoEditingSplineState();

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    void paintBackground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option);

private:
    GraphicsSpline *m_spline;
};
#endif // NO_EDITING_SPLINE_STATE
