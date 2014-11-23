#ifndef SPLINE_STATE
#define SPLINE_STATE

#include "stable.h"

/** Base class for all spline states.
 *  It does nothing, but is not an interface, as the subclasses then only need to override
 *  the methods, they want to react on.
 *  Do not forget, that the hover and mouse methods are simply forwarded form GraphicsSpline
 *  to the SplineState and therefore the GraphicsSpline won't react with default QGraphicsItem
 *  behaviour.
 */
class SplineState {
public:

    virtual ~SplineState();

    virtual QRectF boundingRect() const = 0;
    virtual QPainterPath shape() const = 0;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    // Here the default QGraphicsItem::mouseReleaseEvent is called by the GraphicsScpline
    // unless the event is accepted (event->accept());
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    virtual void receivedClickOn(QPointF scenePos);

    virtual void paintBackground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option);
    virtual void paintForeground(QPen &pen, QPainter *painter, const QStyleOptionGraphicsItem *option);
};

#endif // SPLINE_STATE
