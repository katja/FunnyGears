#ifndef SPLINE
#define SPLINE

#include "stable.h"
#include "Point.h"

class Spline : public QGraphicsItem {

public:
    Spline(QGraphicsItem *parent = 0);
    ~Spline();

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

#endif // SPLINE
