#include "graphics_objects/GraphicsSpline.h"
#include "preferences.h"
#include "helpers.h"

const int GraphicsSpline::Type = GraphicsSpline::UserType + Type::GraphicsSplineType;

bool GraphicsSpline::isGraphicsSplineItem(QGraphicsItem *item) {
    if(item->type() == Type)
        return true;
    return false;
}

GraphicsSpline::GraphicsSpline(GraphicsItem *parent) : GraphicsItem(parent), m_isTangentDrawn(false), m_tangentValue(-1.0f) {
    std::cout << "GraphicsSpline is created" << std::endl;
    m_spline = new Spline();
    int partColor = qrand() % 512 + 100;
    m_color = QColor(partColor / 5, partColor * 2 / 5, partColor % 256);

    setToolTip("Description of what will happen or what to do"); //TODO: Add description
    setFlags( QGraphicsItem::ItemIsMovable
            | QGraphicsItem::ItemIsSelectable
            | QGraphicsItem::ItemSendsGeometryChanges
            );
}

GraphicsSpline::~GraphicsSpline() {
    std::cout << "GraphicsSpline is deleted" << std::endl;
    delete m_spline;
}

GraphicsSpline* GraphicsSpline::copy() const {
    std::cout << "COPY: ";
    GraphicsSpline *copy = new GraphicsSpline(parentItem());
    *(copy->m_spline) = *m_spline;
    copy->m_color = m_color;
    copy->m_isTangentDrawn = m_isTangentDrawn;
    copy->m_tangentValue = m_tangentValue;
    copy->m_points.clear();
    for(int i = 0; i < m_points.size(); ++i) {
        copy->m_points << m_points.at(i);
    }
    return copy;
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
    while(!m_points.empty()) {
        Point *point = m_points.takeLast();
        delete point;
    }
    const QVector<vec2> controlPoints = convertToQVector(m_spline->controlPoints());
    for(int i = 0; i < controlPoints.size(); ++i) {
        Point *p = new Point(this);
        p->setPos(QPointF(controlPoints.at(i)(0), controlPoints.at(i)(1)));
        m_points << p;
    }
}

QRectF GraphicsSpline::boundingRect() const {
    if(m_points.empty()) {
        return QRectF();
    }
    QPointF min, max;
    min = m_points.at(0)->pos();
    max = min;
    for(int i = 1; i < m_points.size(); ++i) {
        setMinAndMax(min, max, m_points.at(i));
    }
    // when updating the spline from selected to not selected the bounding rect would be smaller
    // but painting should clean selection. Therefore little space for selection area is added
    // Normally, half of HighlightedLineWidth should be enough, but a little should not hurt.
    min -= QPointF(Preferences::HighlightedLineWidth, Preferences::HighlightedLineWidth);
    max += QPointF(Preferences::HighlightedLineWidth, Preferences::HighlightedLineWidth);

    QRectF boundingRect = QRectF(min, max);
    if(m_isTangentDrawn) {
        boundingRect |= tangentPath().boundingRect();
    }
    return boundingRect;
}

QPainterPath GraphicsSpline::shape() const {
    //only the control polygon path is used here, as with the spline path this would be too slow
    if(isActive())
        return controlPointPolygonPath(Preferences::HighlightedLineWidth);
    else
        return controlPointPolygonPath(Preferences::SimpleLineWidth);
}

void GraphicsSpline::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);

    painter->setBrush(Qt::NoBrush);

    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    if(option->state & QStyle::State_Selected) {
        if(option->state & QStyle::State_Sunken) {
            pen.setColor(Preferences::ActiveColor);
        } else {
            pen.setColor(Preferences::SelectionColor);
        }
        pen.setWidth(Preferences::HighlightedLineWidth);
        painter->setPen(pen);
        painter->drawPath(controlPointPolygonPath());
        painter->drawPath(splineCurvePath());
    }

    pen.setColor(m_color);
    pen.setWidth(Preferences::SimpleLineWidth);
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);
    painter->drawPath(controlPointPolygonPath());
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawPath(splineCurvePath());

    if(m_isTangentDrawn) {
        pen.setColor(darkenColor(m_color));
        painter->setPen(pen);
        painter->drawPath(tangentPath());
    }
}

QColor GraphicsSpline::color() const {
    return m_color;
}

void GraphicsSpline::addPoint(QPointF scenePos) {
    Point *p = new Point(this);
    p->setPos(mapFromScene(scenePos));
    prepareGeometryChange();
    m_points.append(p);
    m_spline->addControlPoint(p->pos());
}

void GraphicsSpline::removePoint(int index) {
    Point *point = m_points.at(index);
    prepareGeometryChange();
    scene()->removeItem(m_points.at(index));
    m_points.removeAt(index);
    delete point;
    m_spline->removeControlPoint(index);
}

void GraphicsSpline::removePointNear(QPointF scenePos) {
    for(int i = 0; i < m_points.size(); ++i) {
        if((m_points.at(i)->pos() - scenePos).manhattanLength() <= 5) {
            removePoint(i);
        }
    }
}

void GraphicsSpline::pointMoveEvent(Point *point, QGraphicsSceneMouseEvent *event) {
    prepareGeometryChange();
    point->setPos(event->pos() + point->pos());
    m_spline->moveControlPoint(m_points.indexOf(point), point->pos());
}

int GraphicsSpline::type() const {
    return Type;
}

void GraphicsSpline::clickReceived(QPointF point, Editing::State state) {
    std::cout << "GraphicsSpline::clickReceived and state: " << state << std::endl;
    if(state == Editing::Pen)
        addPoint(point);
    else if(state == Editing::Eraser)
        removePointNear(point);
}

QPainterPath GraphicsSpline::controlPointPolygonPath(qreal width) const {
    if(m_points.empty())
        return QPainterPath();

    if(width == 0) {
        QPainterPath controlPointPolygon(m_points.at(0)->pos()); //startpoint
        for(int i = 1; i < m_points.size(); ++i) {
            controlPointPolygon.lineTo(m_points.at(i)->pos());
        }
        return controlPointPolygon;

    } else { // width != 0
        QPainterPath controlPointPolygon;
        for(int i = 1; i < m_points.size(); ++i) {
            QVector2D direction(m_points.at(i)->pos() - m_points.at(i - 1)->pos());
            direction.normalize();
            QPointF additional = -(direction * 0.6 * width).toPointF();
            QPainterPath linePath(m_points.at(i - 1)->pos() + additional + QPointF(-additional.y(), additional.x()));
            linePath.lineTo(m_points.at(i)->pos() - additional + QPointF(-additional.y(), additional.x()));
            linePath.lineTo(m_points.at(i)->pos() - additional - QPointF(-additional.y(), additional.x()));
            linePath.lineTo(m_points.at(i - 1)->pos() + additional - QPointF(-additional.y(), additional.x()));
            linePath.closeSubpath();
            controlPointPolygon.addPath(linePath);
        }
        return controlPointPolygon;
    }
}

QPainterPath GraphicsSpline::splineCurvePath() const {
    if(!m_spline->isValid())
        return QPainterPath();

    vector<QPointF> curve(10 * m_points.size());
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

void GraphicsSpline::adjustInSplineRange(real &value) const {
    if(value < m_spline->lowerDomainLimit()) {
        value = m_spline->lowerDomainLimit();
    } else if (value >= m_spline->upperDomainLimit()) {
        value = m_spline->upperDomainLimit() - 0.01f;
    }
}

void GraphicsSpline::setMinAndMax(QPointF &min, QPointF &max, const Point *point) const {
    QRectF pointRect = point->boundingRect();
    pointRect.translate(point->pos());

    setMin(min, pointRect.bottomLeft());
    setMin(min, pointRect.topRight());
    setMax(max, pointRect.bottomLeft());
    setMax(max, pointRect.topRight());
}

void GraphicsSpline::setMin(QPointF &min, const QPointF point) const {
    if(min.x() > point.x()) min.setX(point.x());
    if(min.y() > point.y()) min.setY(point.y());
}

void GraphicsSpline::setMax(QPointF &max, const QPointF point) const {
    if(max.x() < point.x()) max.setX(point.x());
    if(max.y() < point.y()) max.setY(point.y());
}
