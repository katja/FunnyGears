#ifndef POINT
#define POINT

#include "stable.h"

class GraphicsSpline;

class Point : public QGraphicsItem {

public:
    static qreal radius();

    Point(QGraphicsItem *parent = 0);
    Point(GraphicsSpline *parent);
    ~Point();

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    static qreal M_radius;
    GraphicsSpline *m_parentSpline;
    QColor m_color;

    void initialize();
    bool hasSplineAsParent() const;
};

#endif // POINT
