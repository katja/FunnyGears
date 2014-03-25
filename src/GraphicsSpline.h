#ifndef GRAPHICS_SPLINE
#define GRAPHICS_SPLINE

#include "stable.h"
#include "definitions.h"
#include "Point.h"
#include "Spline.h"

class GraphicsSpline : public QGraphicsItem {

public:
    GraphicsSpline(QGraphicsItem *parent = 0);
    // GraphicsSpline(const GraphicsSpline &other);
    ~GraphicsSpline();

    GraphicsSpline* copy() const;

    const Spline* spline() const;

    void changeDegree(int degree);
    void changeTornToEdges(bool tearToEdges);

    void setTangentDrawn(bool draw);
    bool isTangentDrawn() const;
    void setTangentValue(real);
    real tangentValue();

    void refineSpline();

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QColor color() const;

    void pointMoveEvent(Point *point, QGraphicsSceneMouseEvent *event);

    void addPoint(QPointF scenePos);

private:
    Spline *m_spline;
    QColor m_color;
    QList<Point*> m_points;

    bool m_isTangentDrawn;
    real m_tangentValue;

    QPainterPath controlPointPolygonPath(qreal width = 0) const;
    QPainterPath splineCurvePath() const;
    QPainterPath tangentPath() const;

    void adjustInSplineRange(real &value) const;

    void setMinAndMax(QPointF &min, QPointF &max, const Point *point) const;
    void setMax(QPointF &max, const QPointF point) const;
    void setMin(QPointF &max, const QPointF point) const;

};

#endif // GRAPHICS_SPLINE
