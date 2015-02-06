#include "graphics_widgets/GraphicsSplineGearAttributesWidget.h"

GraphicsSplineGearAttributesWidget::
GraphicsSplineGearAttributesWidget(QWidget *parent) : GraphicsItemAttributesWidget(parent)
{
    m_splineAttributesWidget = new GraphicsSplineAttributesWidget(this);

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); //Sets border to 0, this results in same width as with only using the SplineAttributesWidget
    // layout->setSpacing(0); //uses no space between widgets in layout (not border)
    layout->addWidget(m_splineAttributesWidget, 0, 0, 1, 1);
    setLayout(layout);
}

GraphicsSplineGearAttributesWidget::
~GraphicsSplineGearAttributesWidget()
{}

void GraphicsSplineGearAttributesWidget::
setItem(QGraphicsItem *graphicsItem) {
    m_splineAttributesWidget->setItem(graphicsItem);
    m_currentGear = static_cast<GraphicsSplineGear*>(graphicsItem);
}

bool GraphicsSplineGearAttributesWidget::
worksOnItem(QGraphicsItem *graphicsItem) {
    if(GraphicsSplineGear::isGraphicsSplineGearItem(graphicsItem))
        return true;
    return false;
}
