#include "graphics_objects/GraphicsSimpleGear.h"
#include "preferences.h"
#include "helpers.h"

const int GraphicsSimpleGear::Type = GraphicsSimpleGear::UserType + Type::GraphicsSimpleGearType;

bool GraphicsSimpleGear::isGraphicsSimpleGearItem(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

GraphicsSimpleGear::GraphicsSimpleGear() : GraphicsScheduleItem(), m_simpleGear(0), m_radius(100), m_numberOfTeeth(10) {
    std::cout << "GraphicsSimpleGear is created" << std::endl;

    int partColor = qrand() % 50 + 50;
    m_color = QColor(partColor / 2, partColor, (2 * partColor) % 150);

    setToolTip("Description of what will happen or what to do"); //TODO: Add description
    setFlags( QGraphicsItem::ItemIsMovable
            | QGraphicsItem::ItemIsSelectable
            | QGraphicsItem::ItemSendsGeometryChanges
            );
}

GraphicsSimpleGear::~GraphicsSimpleGear() {
    std::cout << "GraphicsSimpleGear is deleted" << std::endl;
}

int GraphicsSimpleGear::type() const {
    return Type;
}

QString GraphicsSimpleGear::defaultName() const {
    return "Simple Gear";
}

QRectF GraphicsSimpleGear::boundingRect() const {
    real angularPitchStrokesLength = Preferences::AngularPitchStrokesLength;
    if(m_simpleGear != 0 && m_simpleGear->maxRadius() > angularPitchStrokesLength) {
        angularPitchStrokesLength = m_simpleGear->maxRadius();
    }
    QRectF boundingRect = QRectF(-angularPitchStrokesLength, -angularPitchStrokesLength,
                                    2 * angularPitchStrokesLength, 2 * angularPitchStrokesLength);
    return boundingRect;
}

void GraphicsSimpleGear::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
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
        painter->drawPath(angularPitchStrokesPath());
    }

    pen.setColor(m_color);
    pen.setWidth(Preferences::SimpleLineWidth);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawPath(angularPitchStrokesPath());
}

void GraphicsSimpleGear::receivedClickOn(QPointF point) {
    std::cout << "GraphicsSimpleGear::clickReceived" << std::endl;
}

QPainterPath GraphicsSimpleGear::angularPitchStrokesPath() {
    QPainterPath path;
    path.lineTo(0, Preferences::AngularPitchStrokesLength);
    path.moveTo(0,0);
    path.lineTo(0, -Preferences::AngularPitchStrokesLength);
    path.moveTo(0,0);
    path.lineTo(Preferences::AngularPitchStrokesLength, 0);
    path.moveTo(0,0);
    return path;
}
