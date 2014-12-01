#ifndef GRAPHICS_SPLINE
#define GRAPHICS_SPLINE

#include "stable.h"
#include "definitions.h"
#include "preferences.h"
#include "graphics_objects/GraphicsScheduleItem.h"
#include "graphics_objects/SplineState.h"
#include "basic_objects/Spline.h"

class GraphicsSpline : public GraphicsScheduleItem {

public:
    static const int Type;// = GraphicsSpline::UserType + Type::GraphicsSplineType;
    static bool isGraphicsSplineItem(QGraphicsItem *item);

    GraphicsSpline(Spline *spline = 0);
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
    QPointF positionOfPoint(int index);
    /** Tests for each control point if the given point @param localPos lies in a circle
     *  with radius @param radius around the control point. The control points are tested
     *  in increasing order and the index of the first suitable control point is returned.
     *  If no control point is found -1 is returned.
     *  @return The first control point whose circle with radius @radius includes localPos
     */
    int indexOfPointAt(QPointF localPos, qreal radius = Preferences::PointRadius);

    QRectF boundingRect() const override; // from QGraphicsItem
    QRectF normalBoundingRect(qreal controlPointRadius = Preferences::PointRadius + 0.5f * Preferences::HighlightedLineWidth) const;
    QPainterPath shape() const override; // from QGraphicsItem
    QPainterPath normalShape() const;

    QPainterPath controlPointPolygonPath(qreal width = 0) const;
    QPainterPath controlPointsPaths(qreal radius = Preferences::PointRadius) const;
    QPainterPath splineCurvePath() const;
    QPainterPath tangentPath() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;  // from QGraphicsItem
    QColor color() const;

    void receivedClickOn(QPointF scenePos) override; // from GraphicsItem

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override; // from QGraphicsItem
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override; // from QGraphicsItem
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override; // from QGraphicsItem

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override; // from QGraphicsItem
    void mouseMovePointEvent(uint index, QGraphicsSceneMouseEvent *event);
    void mouseMoveWholeSplineEvent(QGraphicsSceneMouseEvent *event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override; // from QGraphicsItem

    void startEditing(Editing::State editingState) override; // from EditingToolBarListener
    void stopEditing() override; // from EditingToolBarListener
    void executeEditingAction(Editing::Action editingAction) override; // from EditingToolBarListener

private:
    Spline *m_spline;

    SplineState *m_state, *m_noEditingState, *m_pointerState, *m_penState, *m_eraserState;

    bool m_isTangentDrawn;
    real m_tangentValue;

    QColor m_color;

    void adjustInSplineRange(real &value) const;

};

#endif // GRAPHICS_SPLINE
