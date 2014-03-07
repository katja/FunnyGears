#include "Spline.h"

Spline::Spline() {
    std::cout << "Spline is created" << std::endl;
    m_color = QColor(qrand() % 256, qrand() % 256, qrand() % 256);

    // setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton);
    setToolTip("Description of what will happen or what to do"); //TODO: Add description

    Point *p = new Point(this);
    p->setPos(qrand() % 80, qrand() % 80);
    m_points.append(p);

    p = new Point(this);
    p->setPos(-(qrand() % 130), qrand() % 80);
    m_points.append(p);

    p = new Point(this);
    p->setPos(-(qrand() % 37), -(qrand() % 90));
    m_points.append(p);

    setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

Spline::~Spline() {
    std::cout << "Spline is deleted" << std::endl;
}

void Spline::setMinAndMax(QPointF &min, QPointF &max, const Point *point) const {
    QRectF pointRect = point->boundingRect();
    pointRect.translate(point->pos());

    setMin(min, pointRect.bottomLeft());
    setMin(min, pointRect.topRight());
    setMax(max, pointRect.bottomLeft());
    setMax(max, pointRect.topRight());
}

void Spline::setMin(QPointF &min, const QPointF point) const {
    if(min.x() < point.x())
        min.setX(point.x());
    if(min.y() < point.y())
        min.setY(point.y());
}

void Spline::setMax(QPointF &max, const QPointF point) const {
    if(max.x() > point.x())
        max.setX(point.x());
    if(max.y() > point.y())
        max.setY(point.y());
}

QRectF Spline::boundingRect() const {
    if(m_points.empty()) {
        return QRectF(0, 0, 0, 0);
    }

    QPointF min, max;

    min = m_points.at(0)->pos();
    max = min;

    for(int i = 1; i < m_points.size(); ++i) {
        setMinAndMax(min, max, m_points.at(i));
    }
    return QRectF(min, max);
}

void Spline::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(m_color, 0));
    painter->setBrush(Qt::NoBrush);

    for(int i = 1; i < m_points.size(); ++i) {
            painter->drawLine(m_points.at(i - 1)->pos(), m_points.at(i)->pos());
    }
}

void Spline::pointMoveEvent(Point *point, QGraphicsSceneMouseEvent *event) {
    prepareGeometryChange();
    point->setPos(event->pos() + point->pos());
}

QColor Spline::color() const {
    return m_color;
}
