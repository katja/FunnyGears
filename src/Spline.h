#ifndef SPLINE
#define SPLINE

#include "stable.h"
#include "Point.h"

class Spline : public QGraphicsItem {

public:
    Spline();
    ~Spline();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void pointMoveEvent(Point *point, QGraphicsSceneMouseEvent *event);

    QColor color() const;

private:
    QColor m_color;
    QList<Point*> m_points;
    static const int M_pointSize = 2;

    void setMinAndMax(QPointF &min, QPointF &max, const Point *point) const;
    void setMax(QPointF &max, const QPointF point) const;
    void setMin(QPointF &max, const QPointF point) const;

};

#endif // SPLINE
