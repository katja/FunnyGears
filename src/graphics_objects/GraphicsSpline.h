#ifndef GRAPHICS_SPLINE
#define GRAPHICS_SPLINE

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/GraphicsScheduleItem.h"
#include "graphics_objects/Point.h"
#include "basic_objects/Spline.h"

class GraphicsSpline : public GraphicsScheduleItem {

public:
    static const int Type;// = GraphicsSpline::UserType + Type::GraphicsSplineType;
    static bool isGraphicsSplineItem(QGraphicsItem *item);

    GraphicsSpline();
    virtual ~GraphicsSpline();

    int type() const override; //from QGraphicsItem
    QString defaultName() const override; //from GraphicsItem

    const Spline* spline() const;

    void changeDegree(int degree);
    void changeTornToEdges(bool tearToEdges);

    void setTangentDrawn(bool draw);
    bool isTangentDrawn() const;
    void setTangentValue(real);
    real tangentValue();

    void refineSpline();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QColor color() const;

    void addPoint(QPointF scenePos);
    void removePoint(int index);
    void removePointNear(QPointF scenePos);
    void pointMoveEvent(Point *point, QGraphicsSceneMouseEvent *event);

    void receivedClickOn(QPointF point) override; //from GraphicsItem
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override; //from QGraphicsItem
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override; //from QGraphicsItem

    void stopEditing() override;
    void startEditing(Editing::State editingState) override;
    void executeEditingAction(Editing::Action editingAction) override;

    void setParentItem(QGraphicsItem *newParent); //override from QGraphicsItem

private:
    Spline *m_spline;
    QList<Point*> m_points;

    bool m_isTangentDrawn;
    real m_tangentValue;

    QColor m_color;
    Editing::State m_editingState;

    QPainterPath controlPointPolygonPath(qreal width = 0) const;
    QPainterPath splineCurvePath() const;
    QPainterPath tangentPath() const;

    void adjustInSplineRange(real &value) const;

    void setMinAndMax(QPointF &min, QPointF &max, const Point *point) const;
    void setMax(QPointF &max, const QPointF point) const;
    void setMin(QPointF &max, const QPointF point) const;

};

#endif // GRAPHICS_SPLINE
