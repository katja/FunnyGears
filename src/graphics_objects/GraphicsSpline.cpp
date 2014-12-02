#include "graphics_objects/GraphicsSpline.h"
#include "helpers.h"
#include "graphics_objects/NoEditingSplineState.h"
#include "graphics_objects/PointerSplineState.h"
#include "graphics_objects/PenSplineState.h"
#include "graphics_objects/EraserSplineState.h"

const int GraphicsSpline::Type = GraphicsSpline::UserType + Type::GraphicsSplineType;

bool GraphicsSpline::isGraphicsSplineItem(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

GraphicsSpline::GraphicsSpline(Spline *spline) : m_isTangentDrawn(false), m_tangentValue(-1.0f) {

    std::cout << "GraphicsSpline is created" << std::endl;

    m_spline = spline;

    m_noEditingState = new NoEditingSplineState(this);
    m_pointerState = new PointerSplineState(this);
    m_penState = new PenSplineState(this);
    m_eraserState = new EraserSplineState(this);
    m_state = m_noEditingState;

    int partColor = qrand() % 512 + 100;
    m_color = QColor(partColor / 5, partColor * 2 / 5, partColor % 256);

    setToolTip("Description of what will happen or what to do"); //TODO: Add description
    setFlags( QGraphicsItem::ItemIsMovable
            | QGraphicsItem::ItemIsSelectable
            | QGraphicsItem::ItemSendsGeometryChanges
            );
    setAcceptHoverEvents(true);
}

GraphicsSpline::~GraphicsSpline() {
    std::cout << "GraphicsSpline is deleted" << std::endl;
    delete m_spline;
    delete m_noEditingState;
    delete m_pointerState;
    delete m_penState;
    delete m_eraserState;
}

int GraphicsSpline::type() const {
    return Type;
}

QString GraphicsSpline::defaultName() const {
    return "Spline";
}

const Spline* GraphicsSpline::spline() const {
    return m_spline;
}

void GraphicsSpline::changeDegree(int degree) {
    prepareGeometryChange();
    m_spline->setDegree(degree);
}

void GraphicsSpline::changeTornToEdges(bool tearToEdges) {
    prepareGeometryChange();
    m_spline->setTornToEdges(tearToEdges);
}

void GraphicsSpline::setTangentDrawn(bool draw) {
    if(draw && m_spline->isValid()) {
        if(m_tangentValue < m_spline->lowerDomainLimit())
            m_tangentValue = m_spline->lowerDomainLimit();
        if(m_tangentValue >= m_spline->upperDomainLimit())
            m_tangentValue = m_spline->upperDomainLimit();

    }
    prepareGeometryChange();
    m_isTangentDrawn = draw;
}

bool GraphicsSpline::isTangentDrawn() const {
    return m_isTangentDrawn;
}

void GraphicsSpline::setTangentValue(real value) {
    adjustInSplineRange(value);
    prepareGeometryChange();
    m_tangentValue = value;
}

real GraphicsSpline::tangentValue() {
    adjustInSplineRange(m_tangentValue);
    return m_tangentValue;
}

void GraphicsSpline::refineSpline() {
    if(!m_spline->isValid())
        return;
    prepareGeometryChange();
    real minDist = (m_spline->upperDomainLimit() - m_spline->lowerDomainLimit()) / m_spline->numberOfControlPoints();
    m_spline->knotRefinement(minDist);
    const vector<vec2> controlPoints = m_spline->controlPoints();
}

void GraphicsSpline::addPoint(QPointF scenePos) {
    QPointF localPosition = mapFromScene(scenePos);
    prepareGeometryChange();
    m_spline->addControlPoint(localPosition.x(), localPosition.y());
    update(boundingRect());
}

void GraphicsSpline::removePoint(int index) {
    prepareGeometryChange();
    m_spline->removeControlPoint(index);
}

void GraphicsSpline::removePointNear(QPointF scenePos) {
    const vector<vec2>& controlPoints = m_spline->controlPoints();
    for(uint i = 0; i < controlPoints.size(); ++i) {
        if((positionOfPoint(i) - scenePos).manhattanLength() <= 5) {
            removePoint(i);
            return;
        }
    }
}

QPointF GraphicsSpline::positionOfPoint(int index) {
    const vector<vec2>& controlPoints = m_spline->controlPoints();
    return QPointF(controlPoints[index].x(), controlPoints[index].y());
}

int GraphicsSpline::indexOfPointAt(QPointF localPos, qreal radius) {
    const vector<vec2> controlPoints = m_spline->controlPoints();
    for(uint i = 0; i < controlPoints.size(); ++i) {
        // localPos is in circle around controlpoint c, if
        // (localPos_x - c_x)^2 + (localPos_y - c_y)^2 <= radius^2
        QPointF c = positionOfPoint(i);
        if((localPos.x() - c.x()) * (localPos.x() - c.x()) + (localPos.y() - c.y()) * (localPos.y() - c.y()) <= radius * radius)
            return i;
    }
    return -1;
}

QRectF GraphicsSpline::boundingRect() const {
    return m_state->boundingRect();
}

QRectF GraphicsSpline::normalBoundingRect(qreal radius) const {
    // Explanation to selection of default radius:
    // When updating the spline from selected to not selected the bounding rect would be smaller
    // but painting should clean selection. Therefore every time the space for the selected curve
    // is used. As the control points form the bounding rect, the additional space for them
    // must be included, too.
    const vector<vec2>& controlPoints = m_spline->controlPoints();
    if(controlPoints.empty()) {
        return QRectF();
    }

    QPointF min, max;
    min = QPointF(controlPoints[0].x(), controlPoints[0].y());
    max = min;
    for(uint i = 1; i < controlPoints.size(); ++i) {
        //calculate bounding rect (is the bounding rect of control points)
        if(controlPoints[i].x() < min.x())
            min.setX(controlPoints[i].x());
        if(controlPoints[i].y() < min.y())
            min.setY(controlPoints[i].y());
        if(controlPoints[i].x() > max.x())
            max.setX(controlPoints[i].x());
        if(controlPoints[i].y() > max.y())
            max.setY(controlPoints[i].y());
    }
    min -= QPointF(radius, radius);
    max += QPointF(radius, radius);

    QRectF boundingRect = QRectF(min, max);
    if(m_isTangentDrawn) {
        boundingRect |= tangentPath().boundingRect();
    }
    return boundingRect;
}

QPainterPath GraphicsSpline::shape() const {
    return m_state->shape();
}

QPainterPath GraphicsSpline::normalShape() const {
    //only the control polygon path is used here, as with the spline path this would be too slow
    if(isActive())
        return controlPointPolygonPath(Preferences::HighlightedLineWidth) + controlPointsPaths(Preferences::PointRadius + 0.5f * Preferences::HighlightedLineWidth);
    else
        return controlPointPolygonPath(Preferences::SimpleLineWidth) + controlPointsPaths(Preferences::PointRadius + 0.5f * Preferences::SimpleLineWidth);
}

QPainterPath GraphicsSpline::controlPointPolygonPath(qreal width) const {
    const vector<vec2> controlPoints = m_spline->controlPoints();
    if(controlPoints.empty())
        return QPainterPath();

    QPainterPath controlPointPolygon;
    controlPointPolygon.addPolygon(QPolygonF(convertToQVectorWithQPointFs(controlPoints)));

    if(width == 0) { // used for painting
        return controlPointPolygon;
    } else { // width != 0, used for shape
        QPainterPathStroker stroker;
        stroker.setWidth(width);
        stroker.setCapStyle(Qt::RoundCap);
        return stroker.createStroke(controlPointPolygon);
    }
}

QPainterPath GraphicsSpline::controlPointsPaths(qreal radius) const {
    const vector<vec2> controlPoints = m_spline->controlPoints();

    QPainterPath pointsPath;
    for(uint i = 0; i < controlPoints.size(); ++i) {
        pointsPath.addEllipse(QPointF(controlPoints[i].x(), controlPoints[i].y()), radius, radius);
    }
    return pointsPath;
}

QPainterPath GraphicsSpline::splineCurvePath() const {
    if(!m_spline->isValid())
        return QPainterPath();
    // try to find out a good sampling value, which allows nice smooth curve,
    // but with as less as possible calculation time. I tried much things, taking into
    // account the size of the bounding rect, the number of control points and so on,
    // but got the most acceptable solution not with much calcuations, but with the following:
    uint samples = static_cast<uint>(ceil(350.0f / m_spline->numberOfControlPoints()));
    // The 300.0f is only an experimentation value. There may be better assumptions, but
    // the one used here, is at least calculated very fast and allows constant/linear
    // calculation time with respect to the control points

    vector<QPointF> curve(samples * m_spline->numberOfControlPoints());
    m_spline->curve(curve);
    QPainterPath path;
    path.addPolygon(QPolygonF(convertToQVector(curve)));
    return path;
}

QPainterPath GraphicsSpline::tangentPath() const {
    if(!m_spline->isValid())
        return QPainterPath();
    qreal tangentValue = m_tangentValue;
    adjustInSplineRange(tangentValue);
    if(m_tangentValue != tangentValue)
        return QPainterPath();

    QPainterPath path;
    vec2 tangentVecPoint = m_spline->evaluate(m_tangentValue);
    vec2 derivative = m_spline->derivative(m_tangentValue);
    vec2 start = tangentVecPoint - 0.5f * Preferences::TangentLength * derivative;
    vec2 stop = tangentVecPoint + 0.5f * Preferences::TangentLength * derivative;
    QVector<QPointF> tangentLine(2);
    tangentLine[0] = QPointF(start(0), start(1));
    tangentLine[1] = QPointF(stop(0), stop(1));
    QPointF tangentPoint = QPointF(tangentVecPoint(0), tangentVecPoint(1));
    path.addEllipse(tangentPoint, Preferences::PointRadius, Preferences::PointRadius);
    path.addPolygon(QPolygonF(tangentLine));
    return path;
}

void GraphicsSpline::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);

    painter->setBrush(Qt::NoBrush);

    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    // Paint Backgound
    pen.setWidth(Preferences::HighlightedLineWidth);
    m_state->paintBackground(pen, painter, option);

    // Paint Foreground
    pen.setWidth(Preferences::SimpleLineWidth);
    pen.setColor(m_color);
    // control polygon:
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);
    painter->drawPath(controlPointPolygonPath());
    // curve:
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawPath(controlPointsPaths());
    painter->drawPath(splineCurvePath());

    //Paint additional foreground
    m_state->paintForeground(pen, painter, option);

    //Paint additional things
    if(m_isTangentDrawn) {
        pen.setColor(darkenColor(m_color));
        painter->setPen(pen);
        painter->drawPath(tangentPath());
    }
}

QColor GraphicsSpline::color() const {
    return m_color;
}

void GraphicsSpline::receivedClickOn(QPointF scenePos) {
    m_state->receivedClickOn(scenePos);
}

void GraphicsSpline::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    m_state->hoverEnterEvent(event);
}

void GraphicsSpline::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    m_state->hoverLeaveEvent(event);
}

void GraphicsSpline::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    m_state->hoverMoveEvent(event);
}

void GraphicsSpline::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    m_state->mouseMoveEvent(event);
}

void GraphicsSpline::mouseMovePointEvent(uint index, QGraphicsSceneMouseEvent *event) {
    prepareGeometryChange();
    m_spline->moveControlPoint(index, event->pos());
}

void GraphicsSpline::mouseMoveWholeSplineEvent(QGraphicsSceneMouseEvent *event) {
    GraphicsItem::mouseMoveEvent(event);
}

void GraphicsSpline::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    m_state->mouseReleaseEvent(event);
    if(!event->isAccepted())
        GraphicsItem::mouseReleaseEvent(event);
}

void GraphicsSpline::startEditing(Editing::State editingState) {
    switch(editingState) {
        case Editing::Pointer:
            m_state = m_pointerState; break;
        case Editing::Pen:
            m_state = m_penState; break;
        case Editing::Eraser:
            m_state = m_eraserState; break;
        default:
            m_state = m_noEditingState;
    }
    update();
}

void GraphicsSpline::stopEditing() {
    m_state = m_noEditingState;
    update();
}

void GraphicsSpline::executeEditingAction(Editing::Action editingAction) {
    Q_UNUSED(editingAction);
}

void GraphicsSpline::adjustInSplineRange(real &value) const {
    if(value < m_spline->lowerDomainLimit()) {
        value = m_spline->lowerDomainLimit();
    } else if (value >= m_spline->upperDomainLimit()) {
        value = m_spline->upperDomainLimit() - 0.01f;
    }
}
