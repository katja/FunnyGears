#include "graphics_objects/GraphicsSplineGear.h"
#include "preferences.h"
#include "helpers.h"

const int GraphicsSplineGear::Type = GraphicsSplineGear::UserType + Type::GraphicsSplineGearType;

bool GraphicsSplineGear::isGraphicsSplineGearItem(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

GraphicsSplineGear::GraphicsSplineGear() : m_splineGear(0) {
    std::cout << "GraphicsSplineGear is created" << std::endl;

    int partColor = qrand() % 50 + 50;
    m_color = QColor(partColor / 2, partColor, (2 * partColor) % 150);

    setToolTip("Description of what will happen or what to do"); //TODO: Add description
    setFlags( QGraphicsItem::ItemIsMovable
            | QGraphicsItem::ItemIsSelectable
            | QGraphicsItem::ItemSendsGeometryChanges
            );
}

GraphicsSplineGear::~GraphicsSplineGear() {
    std::cout << "GraphicsSplineGear is deleted" << std::endl;
}

int GraphicsSplineGear::type() const {
    return Type;
}

QString GraphicsSplineGear::defaultName() const {
    return "Spline Gear";
}

QRectF GraphicsSplineGear::boundingRect() const {
    real maxRadius = Preferences::AngularPitchStrokesLength;
    if(m_splineGear != 0 && m_splineGear->maximumDistanceToCenter() > maxRadius) {
        maxRadius = m_splineGear->maximumDistanceToCenter();
    }
    return QRectF(-maxRadius, -maxRadius, 2 * maxRadius, 2 * maxRadius);
}

// void GraphicsSplineGear::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
//     Q_UNUSED(widget);

//     painter->setBrush(Qt::NoBrush);

//     QPen pen;
//     pen.setCapStyle(Qt::RoundCap);
//     pen.setJoinStyle(Qt::RoundJoin);

//     // Paint Backgound
//     pen.setWidth(Preferences::HighlightedLineWidth);
//     m_state->paintBackground(pen, painter, option);

//     // Paint Foreground
//     pen.setWidth(Preferences::SimpleLineWidth);
//     pen.setColor(m_color);
//     // control polygon:
//     pen.setStyle(Qt::DotLine);
//     painter->setPen(pen);
//     painter->drawPath(controlPointPolygonPath());
//     // curve:
//     pen.setStyle(Qt::SolidLine);
//     painter->setPen(pen);
//     painter->drawPath(controlPointsPaths());
//     painter->drawPath(splineCurvePath());

//     //Paint additional foreground
//     m_state->paintForeground(pen, painter, option);

//     //Paint additional things
//     if(m_isTangentDrawn) {
//         pen.setColor(darkenColor(m_color));
//         painter->setPen(pen);
//         painter->drawPath(tangentPath());
//     }
// }

void GraphicsSplineGear::receivedClickOn(QPointF scenePos) {
    std::cout << "GraphicsSplineGear::clickReceived" << std::endl;
}

QPainterPath GraphicsSplineGear::angularPitchStrokesPath() {
    QPainterPath path;
    path.lineTo(0, Preferences::AngularPitchStrokesLength);
    path.moveTo(0,0);
    path.lineTo(0, -Preferences::AngularPitchStrokesLength);
    path.moveTo(0,0);
    path.lineTo(Preferences::AngularPitchStrokesLength, 0);
    path.moveTo(0,0);
    return path;
}
