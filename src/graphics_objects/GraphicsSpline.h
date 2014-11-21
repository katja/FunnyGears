#ifndef GRAPHICS_SPLINE
#define GRAPHICS_SPLINE

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/GraphicsScheduleItem.h"
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

    void addPoint(QPointF scenePos);
    void removePoint(int index);
    void removePointNear(QPointF scenePos);

    QRectF boundingRect() const override; // from QGraphicsItem
    QPainterPath shape() const override; // from QGraphicsItem

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;  // from QGraphicsItem
    QColor color() const;

    void receivedClickOn(QPointF scenePos) override; // from GraphicsItem
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override; // from QGraphicsItem
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override; // from QGraphicsItem

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override; // from QGraphicsItem
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override; // from QGraphicsItem

    void mouseMoveEvent(QGraphicsSceneMouseEvent  *event) override; // from QGraphicsItem

    void startEditing(Editing::State editingState) override; // from EditingToolBarListener
    void stopEditing() override; // from EditingToolBarListener
    void executeEditingAction(Editing::Action editingAction) override; // from EditingToolBarListener

private:
    Spline *m_spline;

    bool m_isTangentDrawn;
    real m_tangentValue;

    QColor m_color;
    bool m_mouseIsOverEdge;
    bool m_mouseIsOverPoint;
    int m_indexOfPointUnderMouse;
    Editing::State m_editingState;

    QPainterPath controlPointPolygonPath(qreal width = 0) const;
    QPainterPath controlPointsPaths() const;
    QPainterPath splineCurvePath() const;
    QPainterPath tangentPath() const;

    void findPointAt(QPointF localPos);

    void adjustInSplineRange(real &value) const;

};

#endif // GRAPHICS_SPLINE
