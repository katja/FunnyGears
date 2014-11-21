#include "graphics_objects/GraphicsSpline.h"
#include "preferences.h"
#include "helpers.h"

const int GraphicsSpline::Type = GraphicsSpline::UserType + Type::GraphicsSplineType;

bool GraphicsSpline::isGraphicsSplineItem(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

GraphicsSpline::GraphicsSpline() : m_isTangentDrawn(false), m_tangentValue(-1.0f),
    m_mouseIsOverEdge(false), m_mouseIsOverPoint(false) , m_indexOfPointUnderMouse(-1) {

    std::cout << "GraphicsSpline is created" << std::endl;
    m_spline = new Spline();
    int partColor = qrand() % 512 + 100;
    m_color = QColor(partColor / 5, partColor * 2 / 5, partColor % 256);
    m_editingState = Editing::NoEditing;

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
        if((QPointF(controlPoints[i].x(), controlPoints[i].y()) - scenePos).manhattanLength() <= 5) {
            removePoint(i);
            return;
        }
    }
}

QRectF GraphicsSpline::boundingRect() const {
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
    // when updating the spline from selected to not selected the bounding rect would be smaller
    // but painting should clean selection. Therefore every time the space for the selected curve
    // is used. As the control points form the bounding rect, the additional space for them
    // must be included, too.
    qreal additional = Preferences::HighlightedLineWidth / 2.0f + Preferences::PointRadius;
    min -= QPointF(additional, additional);
    max += QPointF(additional, additional);

    QRectF boundingRect = QRectF(min, max);
    if(m_isTangentDrawn) {
        boundingRect |= tangentPath().boundingRect();
    }
    return boundingRect;
}

QPainterPath GraphicsSpline::shape() const {
    //only the control polygon path is used here, as with the spline path this would be too slow
    if(isActive())
        return controlPointPolygonPath(Preferences::HighlightedLineWidth) + controlPointsPaths();
    else
        return controlPointPolygonPath(Preferences::SimpleLineWidth) + controlPointsPaths();
}

void GraphicsSpline::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);

    painter->setBrush(Qt::NoBrush);

    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    if(option->state & QStyle::State_Selected || m_mouseIsOverEdge || m_mouseIsOverPoint) {
        if(option->state & QStyle::State_Sunken) {
            pen.setColor(Preferences::ActiveColor);
        } else if (m_mouseIsOverEdge) {
            pen.setColor(Preferences::HoverColor);
        } else { // no edge hover and selected
            pen.setColor(Preferences::SelectionColor);
        }
        pen.setWidth(Preferences::HighlightedLineWidth);
        painter->setPen(pen);
        painter->drawPath(controlPointPolygonPath());
        painter->drawPath(controlPointsPaths());
        painter->drawPath(splineCurvePath());

        if(m_mouseIsOverPoint) {
            pen.setWidth(Preferences::HighlightedLineWidth);
            pen.setColor(Preferences::HoverColor);
            painter->setPen(pen);
            vec2 center = m_spline->controlPoints().at(m_indexOfPointUnderMouse);
            painter->drawEllipse(QPointF(center.x(), center.y()), Preferences::PointRadius, Preferences::PointRadius);
        }
    }

    pen.setColor(m_color);
    pen.setWidth(Preferences::SimpleLineWidth);
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);
    painter->drawPath(controlPointPolygonPath());
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawPath(controlPointsPaths());
    painter->drawPath(splineCurvePath());

    if(m_isTangentDrawn) {
        pen.setColor(darkenColor(m_color));
        painter->setPen(pen);
        painter->drawPath(tangentPath());
    }

    //TODO: remove following:
    if(m_mouseIsOverEdge) {
        pen.setWidth(0);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }
}

QColor GraphicsSpline::color() const {
    return m_color;
}

void GraphicsSpline::receivedClickOn(QPointF scenePos) {
    std::cout << "GraphicsSpline::receivedClickOn and state: " << m_editingState << std::endl;
    if(m_editingState == Editing::Pen)
        addPoint(scenePos);
    else if(m_editingState == Editing::Eraser)
        removePointNear(scenePos);
}

void GraphicsSpline::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    // std::cout << "GraphicsSpline::mousePressEvent() tracked, nothing done, forwarded to QGraphicsItem" << std::endl;
    QGraphicsItem::mousePressEvent(event);
}
void GraphicsSpline::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    // std::cout << "GraphicsSpline::mouseReleaseEvent() tracked, nothing done, forwarded to QGraphicsItem" << std::endl;
    QGraphicsItem::mouseReleaseEvent(event);
}

void GraphicsSpline::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    if(controlPointsPaths().contains(event->pos())) {
        std::cout << "GraphicsSpline::hoverEnterEvent on point" << std::endl;
        m_mouseIsOverPoint = true;
        findPointAt(event->pos());
    } else {
        std::cout << "GraphicsSpline::hoverEnterEvent whole spline" << std::endl;
        m_mouseIsOverEdge = true;
    }
    update();
}

void GraphicsSpline::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    std::cout << "GraphicsSpline::hoverLeaveEvent" << std::endl;
    m_mouseIsOverEdge = false;
    m_mouseIsOverPoint = false;
    update();
}


void GraphicsSpline::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(m_mouseIsOverPoint) {
        prepareGeometryChange();
        m_spline->moveControlPoint(m_indexOfPointUnderMouse, event->pos());
    } else {
        GraphicsItem::mouseMoveEvent(event);
    }
}

void GraphicsSpline::startEditing(Editing::State editingState) {
    if(editingState == Editing::Pointer ||
        editingState == Editing::Pen ||
        editingState == Editing::Eraser)
        m_editingState = editingState;
    else
        stopEditing();
}

void GraphicsSpline::stopEditing() {
    m_editingState = Editing::NoEditing;
}

void GraphicsSpline::executeEditingAction(Editing::Action editingAction) {
    Q_UNUSED(editingAction);
}

QPainterPath GraphicsSpline::controlPointPolygonPath(qreal width) const {
    const vector<vec2> controlPoints = m_spline->controlPoints();
    if(controlPoints.empty())
        return QPainterPath();

    QPainterPath controlPointPolygon(QPointF(controlPoints[0].x(), controlPoints[0].y())); //startpoint
    for(uint i = 1; i < controlPoints.size(); ++i) {
        controlPointPolygon.lineTo(QPointF(controlPoints[i].x(), controlPoints[i].y()));
    }

    if(width == 0) { // used for painting
        return controlPointPolygon;
    } else { // width != 0, used for shape
        QPainterPathStroker stroker;
        stroker.setWidth(width);
        stroker.setCapStyle(Qt::RoundCap);
        return stroker.createStroke(controlPointPolygon);
    }
    //TODO: is the above the faster one, or the below??? Test it!!!

    // if(width == 0) { // used for painting
    //     QPainterPath controlPointPolygon(QPointF(controlPoints[0].x(), controlPoints[0].y())); //startpoint
    //     for(uint i = 1; i < controlPoints.size(); ++i) {
    //         controlPointPolygon.lineTo(QPointF(controlPoints[i].x(), controlPoints[0].y()));
    //     }
    //     return controlPointPolygon;

    // } else { // width != 0, used for shape
    //     QPainterPath controlPointPolygon;
    //     for(uint i = 1; i < controlPoints.size(); ++i) {
    //         QPointF b = QPointF(controlPoints[i].x(), controlPoints[i].y());
    //         QPointF a = QPointF(controlPoints[i - 1].x(), controlPoints[i - 1].y());
    //         QVector2D direction(b - a);
    //         direction.normalize();
    //         QPointF additional = -(direction * 0.6 * width).toPointF();
    //         QPainterPath linePath(a + additional + QPointF(-additional.y(), additional.x()));
    //         linePath.lineTo(b - additional + QPointF(-additional.y(), additional.x()));
    //         linePath.lineTo(b - additional - QPointF(-additional.y(), additional.x()));
    //         linePath.lineTo(a + additional - QPointF(-additional.y(), additional.x()));
    //         linePath.closeSubpath();
    //         controlPointPolygon.addPath(linePath);
    //     }
    //     return controlPointPolygon;
    // }
}

QPainterPath GraphicsSpline::controlPointsPaths() const {
    const vector<vec2> controlPoints = m_spline->controlPoints();

    QPainterPath pointsPath;
    qreal r = Preferences::PointRadius;
    for(uint i = 0; i < controlPoints.size(); ++i) {
        pointsPath.addEllipse(QPointF(controlPoints[i].x(), controlPoints[i].y()), r, r);
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
    uint samples = static_cast<uint>(ceil(300.0f / m_spline->numberOfControlPoints()));
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

void GraphicsSpline::findPointAt(QPointF localPos) {
    const vector<vec2> controlPoints = m_spline->controlPoints();
    for(uint i = 0; i < controlPoints.size(); ++i) {
        QPainterPath ellipse;
        qreal r = Preferences::PointRadius;
        ellipse.addEllipse(QPointF(controlPoints[i].x(), controlPoints[i].y()), r, r);
        if(ellipse.contains(localPos)) {
            m_indexOfPointUnderMouse = i;
            return;
        }
    }
}

void GraphicsSpline::adjustInSplineRange(real &value) const {
    if(value < m_spline->lowerDomainLimit()) {
        value = m_spline->lowerDomainLimit();
    } else if (value >= m_spline->upperDomainLimit()) {
        value = m_spline->upperDomainLimit() - 0.01f;
    }
}
