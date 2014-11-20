#ifndef POINT
#define POINT

#include "stable.h"

class GraphicsSpline;

class Point : public QGraphicsItem {

public:
    static qreal radius();

    Point(QGraphicsItem *parent = 0);
    Point(GraphicsSpline *parent);
    virtual ~Point();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    GraphicsSpline *m_parentSpline;
    QColor m_color;

    void initialize();
    bool hasSplineAsParent() const;
};

#endif // POINT
