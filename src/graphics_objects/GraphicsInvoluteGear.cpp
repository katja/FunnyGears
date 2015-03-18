#include "helpers.h"
#include "preferences.h"
#include "graphics_objects/GraphicsInvoluteGear.h"

const int GraphicsInvoluteGear::Type = GraphicsInvoluteGear::UserType + Type::GraphicsInvoluteGearType;

bool GraphicsInvoluteGear::isGraphicsInvoluteGearItem(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

GraphicsInvoluteGear::GraphicsInvoluteGear() : GraphicsScheduleItem() {
}

GraphicsInvoluteGear::~GraphicsInvoluteGear() {
}

QRectF GraphicsInvoluteGear::boundingRect() const {
    return QRectF(-10, -10, 20, 20);
    //TODO!!!
}

void GraphicsInvoluteGear::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    painter->drawEllipse(-5, -5, 10, 10);
    //TODO!!!
}

int GraphicsInvoluteGear::type() const {
    return Type;
}

void GraphicsInvoluteGear::receivedClickOn(QPointF scenePos) {
}

QString GraphicsInvoluteGear::defaultName() const {
    return "Involute Gear";
}
