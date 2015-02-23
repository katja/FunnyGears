#include "GraphicsMatingSplineGear.h"
#include "graphics_objects/GraphicsGearPair.h"

GraphicsMatingSplineGear::GraphicsMatingSplineGear(SplineGear *gear, GraphicsGearPair *parent) :
    GraphicsSplineGear(gear),
    m_graphicsGearPair(parent)
{
    std::cout << "GraphicsMatingSplineGear is created" << std::endl;
    setParentItem(m_graphicsGearPair);
}

GraphicsMatingSplineGear::~GraphicsMatingSplineGear() {
}

void GraphicsMatingSplineGear::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    GraphicsSplineGear::paint(painter, option, widget);
    m_graphicsGearPair->paintAdditionals(painter, this);
}
