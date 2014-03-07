#ifndef POINT
#define POINT

#include "stable.h"

class Spline;

class Point : public QGraphicsItem {

public:
    static qreal radius();

    Point(QGraphicsItem *parent = 0);
    Point(Spline *parent);
    ~Point();


    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    static qreal M_radius;
    Spline *m_parentSpline;
    QColor m_color;

    void initialize();
    bool hasSplineAsParent() const;
};

#endif // POINT
