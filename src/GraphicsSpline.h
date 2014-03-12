#ifndef GRAPHICS_SPLINE
#define GRAPHICS_SPLINE

#include "stable.h"
#include "Point.h"

class GraphicsSpline : public QGraphicsItem {

public:
    GraphicsSpline(QGraphicsItem *parent = 0);
    ~GraphicsSpline();

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void pointMoveEvent(Point *point, QGraphicsSceneMouseEvent *event);

    QColor color() const;

    void addPoint(QPointF point);

private:
    QColor m_color;
    QList<Point*> m_points;
    static const int M_pointSize = 2;

    QPainterPath path(qreal width = 0.0) const;

    void setMinAndMax(QPointF &min, QPointF &max, const Point *point) const;
    void setMax(QPointF &max, const QPointF point) const;
    void setMin(QPointF &max, const QPointF point) const;

};

#endif // GRAPHICS_SPLINE
