#include "GraphicsGearPair.h"
#include "preferences.h"

#include "basic_objects/ContactPoint.h"

const int GraphicsGearPair::Type = GraphicsItem::UserType + Type::GraphicsGearPairType;

bool GraphicsGearPair::isGraphicsGearPair(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

GraphicsGearPair::GraphicsGearPair(GearPair *gearPair) : m_gearPair(gearPair) {
}

GraphicsGearPair::~GraphicsGearPair() {
}

int GraphicsGearPair::type() const {
    return Type;
}


QString GraphicsGearPair::defaultName() const {
    return "Gear Pair";
}
void GraphicsGearPair::receivedClickOn(QPointF scenePos) {
    Q_UNUSED(scenePos);
}

QRectF GraphicsGearPair::boundingRect() const {
    std::list<ContactPoint> *points = m_gearPair->foundPoints();
    vec2 topLeft = points->front().point;
    vec2 bottomRight = topLeft;

    for(ContactPoint point : (*points)) {
        if(topLeft.x > point.point.x)
            topLeft = vec2(point.point.x, topLeft.y);
        if(topLeft.y > point.point.y)
            topLeft = vec2(topLeft.x, point.point.y);
        if(bottomRight.x < point.point.x)
            bottomRight = vec2(point.point.x, bottomRight.y);
        if(bottomRight.y < point.point.y)
            bottomRight = vec2(bottomRight.x, point.point.y);
    }
    return QRectF(QPointF(topLeft.x - 20.0, topLeft.y - 20.0), QPointF(bottomRight.x + 20.0, bottomRight.y + 20.0)); //20.0 is the normal Length at the moment
}

void GraphicsGearPair::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    Spline *s = m_gearPair->completeToothProfile();
    uint samples = s->numberOfControlPoints() * 2 + 400;
    vector<QPointF> curve(samples);
    s->curve(curve);
    QPainterPath path;
    path.addPolygon(QPolygonF(convertToQVector(curve)));
    painter->drawPath(path);

    painter->drawRect(boundingRect());

    std::list<ContactPoint> *points = m_gearPair->foundPoints();
    std::list< PositionList* > *positionLists = m_gearPair->pointsInSortedLists();

    uint r = 10;
    uint g = 0;
    uint b = 50;
    uint red = r;
    uint green = g;
    uint blue = b;

    real distance = m_gearPair->getDistanceOfCenters();

    for(PositionList *list : (*positionLists)) {
        ContactPoint lastCP = list->list.front();
        if(list->position != 0) {
            red = green = blue = 0;
            // QColor fillColor = QColor(128 + list->position * 10, 0, 0);
            // painter->setBrush(QBrush(fillColor));
        }
        for(ContactPoint cp : list->list) {
            paintContactPoint(cp, painter, QColor(red, green, blue));

            b = (b + 8) & 0x1FF;
            if((b & 0x100) > 0)//count downwards
                blue = 0x100 - ((b & 0xFF) + 1);
            else
                blue = b;

            if((b & 0x180) > 0) {
                r = (r + 8) & 0x1FF;
                if((r & 0x100) > 0)//count downwards
                    red = 0x100 - ((r & 0xFF) + 1);
                else
                    red = r;
            }

            if((r & 0x180) > 0) {
                g = (g + 8) & 0x1FF;
                if((g & 0x100) > 0)//count downwards
                    green = 0x100 - ((g & 0xFF) + 1);
                else
                    green = g;
            }
            if(lastCP.originPoint != cp.originPoint) { //not first list element
                painter->drawLine(lastCP.point.x + distance, lastCP.point.y, cp.point.x + distance, cp.point.y);
                lastCP = cp;
            }
        }
    }
}


void GraphicsGearPair::paintContactPoint(ContactPoint point, QPainter *painter, QColor color) {
    painter->save();
    painter->setPen(color);
    if(point.error != ErrorCode::NO_ERROR) {
        painter->setPen(QColor(220, 0, 20)); //red
    }

    real distance = m_gearPair->getDistanceOfCenters();

    //point, normal, originPoint, originNormal
    painter->drawEllipse(QPointF(point.point.x + distance, point.point.y), Preferences::PointRadius, Preferences::PointRadius);
    painter->drawEllipse(QPointF(point.originPoint.x, point.originPoint.y), Preferences::PointRadius, Preferences::PointRadius);
    vec2 endNormal = point.point + point.normal * point.forbiddenAreaLength;
    painter->drawLine(point.point.x + distance, point.point.y, endNormal.x + distance, endNormal.y);
    endNormal = point.originPoint - point.originNormal * point.forbiddenAreaLength;
    painter->drawLine(point.originPoint.x, point.originPoint.y, endNormal.x, endNormal.y);

    painter->setBrush(QBrush(color));
    painter->drawEllipse(QPointF(point.contactPosition.x, point.contactPosition.y), Preferences::PointRadius, Preferences::PointRadius);
    endNormal = point.contactPosition + point.normalInContact * 20.0;
    painter->drawLine(point.contactPosition.x, point.contactPosition.y, endNormal.x, endNormal.y);
    painter->setBrush(QBrush(Qt::NoBrush));

    painter->restore();
}
