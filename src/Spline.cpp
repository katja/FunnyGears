#include "Spline.h"
#include "Preferences.h"

Spline::Spline(QGraphicsItem *parent) : QGraphicsItem(parent) {
    std::cout << "Spline is created" << std::endl;
    m_color = QColor(qrand() % 256, qrand() % 256, qrand() % 256);

    setCursor(Qt::OpenHandCursor);
    setToolTip("Description of what will happen or what to do"); //TODO: Add description
    setFlags( QGraphicsItem::ItemIsMovable
            | QGraphicsItem::ItemIsSelectable
            | QGraphicsItem::ItemSendsGeometryChanges
            );

    // setAcceptedMouseButtons(Qt::LeftButton);

    Point *p = new Point(this);
    p->setPos(qrand() % 80, qrand() % 80);
    m_points.append(p);

    p = new Point(this);
    p->setPos(-(qrand() % 130), qrand() % 80);
    m_points.append(p);

    p = new Point(this);
    p->setPos(-(qrand() % 37), -(qrand() % 90));
    m_points.append(p);

    p = new Point(this);
    p->setPos(-(qrand() % 37), qrand() % 30 + 50);
    m_points.append(p);
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
        return QRectF();
    }

    QPointF min, max;

    min = m_points.at(0)->pos();
    max = min;

    for(int i = 1; i < m_points.size(); ++i) {
        setMinAndMax(min, max, m_points.at(i));
    }
    return QRectF(min, max);
}

QPainterPath Spline::shape() const {
    if(isActive())
        return path(Preferences::HighlightedLineWidth);
    else
        return path(Preferences::SimpleLineWidth);
}

void Spline::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
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
        painter->drawPath(path());
    }

    pen.setColor(m_color);
    pen.setWidth(Preferences::SimpleLineWidth);
    painter->setPen(pen);
    painter->drawPath(path());
}

QPainterPath Spline::path(qreal width) const {
    if(m_points.empty())
        return QPainterPath();

    if(width == 0) {
        QPainterPath path(m_points.at(0)->pos()); //startpoint
        for(int i = 1; i < m_points.size(); ++i) {
            path.lineTo(m_points.at(i)->pos());
        }
        return path;

    } else { // width != 0
        QPainterPath path;
        for(int i = 1; i < m_points.size(); ++i) {
            QVector2D direction(m_points.at(i)->pos() - m_points.at(i - 1)->pos());
            direction.normalize();
            QPointF additional = -(direction * 0.6 * width).toPointF();
            QPainterPath linePath(m_points.at(i - 1)->pos() + additional + QPointF(-additional.y(), additional.x()));
            linePath.lineTo(m_points.at(i)->pos() - additional + QPointF(-additional.y(), additional.x()));
            linePath.lineTo(m_points.at(i)->pos() - additional - QPointF(-additional.y(), additional.x()));
            linePath.lineTo(m_points.at(i - 1)->pos() + additional - QPointF(-additional.y(), additional.x()));
            linePath.closeSubpath();
            path.addPath(linePath);
        }
        return path;
    }
}

void Spline::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mouseMoveEvent(event);
}

void Spline::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Spline::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void Spline::pointMoveEvent(Point *point, QGraphicsSceneMouseEvent *event) {
    prepareGeometryChange();
    point->setPos(event->pos() + point->pos());
}

QColor Spline::color() const {
    return m_color;
}
