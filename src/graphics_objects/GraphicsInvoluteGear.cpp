#include "graphics_objects/GraphicsInvoluteGear.h"
#include "helpers.h"
#include "preferences.h"

const int GraphicsInvoluteGear::Type = GraphicsInvoluteGear::UserType + Type::GraphicsInvoluteGearType;

bool GraphicsInvoluteGear::isGraphicsInvoluteGearItem(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

GraphicsInvoluteGear::GraphicsInvoluteGear() : GraphicsScheduleItem() {
    std::cout << "GraphicsInvoluteGear is created" << std::endl;
}

GraphicsInvoluteGear::~GraphicsInvoluteGear() {
    std::cout << "GraphicsInvoluteGear is deleted" << std::endl;
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
    std::cout << "GraphicsInvoluteGear received a click at " << scenePos << std::endl;
}

QString GraphicsInvoluteGear::defaultName() const {
    return "Involute Gear";
}
