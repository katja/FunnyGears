#ifndef GRAPHICS_SPLINE
#define GRAPHICS_SPLINE

#include "stable.h"
#include "definitions.h"
#include "GraphicsItem.h"
#include "Point.h"
#include "Spline.h"

class GraphicsSpline : public GraphicsItem {

public:
    static const int Type;// = GraphicsSpline::UserType + Type::GraphicsSplineType;
    static bool isGraphicsSplineItem(QGraphicsItem *item);

    GraphicsSpline(GraphicsItem *parent = 0);
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

    void addPoint(QPointF scenePos);
    void removePoint(int index);
    void removePointNear(QPointF scenePos);
    void pointMoveEvent(Point *point, QGraphicsSceneMouseEvent *event);

    int type() const;
    void clickReceived(QPointF point, Editing::State state);

private:
    Spline *m_spline;
    QList<Point*> m_points;

    bool m_isTangentDrawn;
    real m_tangentValue;

    QColor m_color;

    QPainterPath controlPointPolygonPath(qreal width = 0) const;
    QPainterPath splineCurvePath() const;
    QPainterPath tangentPath() const;

    void adjustInSplineRange(real &value) const;

    void setMinAndMax(QPointF &min, QPointF &max, const Point *point) const;
    void setMax(QPointF &max, const QPointF point) const;
    void setMin(QPointF &max, const QPointF point) const;

};

#endif // GRAPHICS_SPLINE
