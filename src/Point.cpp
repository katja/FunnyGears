#include "Point.h"
#include "Spline.h"

qreal Point::M_radius = 2;

qreal Point::radius() {
    return Point::M_radius;
}

qreal Point::M_strokeWidth = 1;

Point::Point(QGraphicsItem *parent) : QGraphicsItem(parent), m_parentSpline(0), m_color(QColor(0, 0, 0)) {
    std::cout << "Point is created     (by QGraphicsItem constructor)" << std::endl;
    initialize();
}

Point::Point(Spline *parent) : QGraphicsItem(parent) {
    std::cout << "Point is created     (by Spline constructor)" << std::endl;
    m_parentSpline = parent;
    m_color = parent->color();
    initialize();
}

void Point::initialize() {
    setCursor(Qt::OpenHandCursor);
    setToolTip("This is a Point-Tooltip!"); //TODO: Add description
    setFlags( QGraphicsItem::ItemIsMovable
            | QGraphicsItem::ItemIsSelectable
            | QGraphicsItem::ItemSendsGeometryChanges
            );
}

Point::~Point() {
    std::cout << "Point is deleted" << std::endl;
}

QRectF Point::boundingRect() const {
    qreal radius = M_radius + 0.5 * M_strokeWidth;
    if(isSelected()) {
        radius = M_radius + 1.5 * M_strokeWidth;
    }
    return QRectF(-radius, -radius, 2 * radius, 2 * radius);
}

QPainterPath Point::shape() const {
    QPainterPath path;
    path.addEllipse(-M_radius, -M_radius, 2 * M_radius, 2 * M_radius);
    return path;
}

void Point::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);

    if(option->state & QStyle::State_Selected) {
        if(option->state & QStyle::State_Sunken) {
            painter->setBrush(QColor(Qt::red));
        } else {
            painter->setBrush(QColor(Qt::yellow));
        }
        painter->setPen(Qt::NoPen);
        qreal bigRadius = M_radius + 1.5 * M_strokeWidth;
        painter->drawEllipse(QPointF(0, 0), bigRadius, bigRadius);
    }

    painter->setPen(QPen(m_color, M_strokeWidth));
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(QPointF(0, 0), M_radius, M_radius);
}

void Point::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(hasSplineAsParent()) {
        m_parentSpline->pointMoveEvent(this, event);
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void Point::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Point::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

bool Point::hasSplineAsParent() const {
    return parentItem() && m_parentSpline && m_parentSpline == parentItem();
}
