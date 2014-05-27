#include "Point.h"
#include "GraphicsSpline.h"
#include "preferences.h"

Point::Point(QGraphicsItem *parent) : QGraphicsItem(parent), m_parentSpline(0), m_color(QColor(0, 0, 0)) {
    std::cout << "Point is created     (by QGraphicsItem constructor)" << std::endl;
    initialize();
}

Point::Point(GraphicsSpline *parent) : QGraphicsItem(parent) {
    std::cout << "Point is created     (by GraphicsSpline constructor)" << std::endl;
    m_parentSpline = parent;
    m_color = parent->color();
    initialize();
}

void Point::initialize() {
    // setCursor(Qt::OpenHandCursor); ATTENTION: do NOT uncomment this!!! Otherwise the cursor will not work! (Please, tell me why *aaaaah*)
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
    qreal radius = Preferences::PointRadius + 0.5 * Preferences::SimpleLineWidth;
    if(isSelected()) {
        radius = Preferences::PointRadius + 0.5 * Preferences::HighlightedLineWidth;
    }
    return QRectF(-radius, -radius, 2 * radius, 2 * radius);
}

QPainterPath Point::shape() const {
    QPainterPath path;
    path.addEllipse(-Preferences::PointRadius, -Preferences::PointRadius, 2 * Preferences::PointRadius, 2 * Preferences::PointRadius);
    return path;
}

void Point::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);

    if(option->state & QStyle::State_Selected) {
        if(option->state & QStyle::State_Sunken) {
            painter->setBrush(Preferences::ActiveColor);
        } else {
            painter->setBrush(Preferences::SelectionColor);
        }
        painter->setPen(Qt::NoPen);
        qreal bigRadius = Preferences::PointRadius + 0.5 * Preferences::HighlightedLineWidth;
        painter->drawEllipse(QPointF(0, 0), bigRadius, bigRadius);
    }

    painter->setPen(QPen(m_color, Preferences::SimpleLineWidth));
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(QPointF(0, 0), Preferences::PointRadius, Preferences::PointRadius);
}

void Point::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(hasSplineAsParent()) {
        m_parentSpline->pointMoveEvent(this, event);
    }
    QGraphicsItem::mouseMoveEvent(event);
}

bool Point::hasSplineAsParent() const {
    return parentItem() && m_parentSpline && m_parentSpline == parentItem();
}
