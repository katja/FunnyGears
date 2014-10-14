#include "GraphicsInvoluteGear.h"
#include "helpers.h"
#include "preferences.h"

const int GraphicsInvoluteGear::Type = GraphicsInvoluteGear::UserType + Type::GraphicsInvoluteGearType;

bool GraphicsInvoluteGear::isGraphicsInvoluteGearItem(QGraphicsItem *item) {
    if(item->type() == Type)
        return true;
    return false;
}

GraphicsInvoluteGear::GraphicsInvoluteGear(GraphicsItem *parent) : GraphicsItem(parent) {
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

void GraphicsInvoluteGear::clickReceived(QPointF point, Editing::State state) {
    std::cout << "GraphicsInvoluteGear received a click at " << point << std::endl;
}

QString GraphicsInvoluteGear::defaultName() const {
    return "Involute Gear";
}
