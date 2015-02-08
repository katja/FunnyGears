#include "graphics_widgets/GraphicsSplineGearAttributesWidget.h"
#include "basic_objects/SplineGear.h"

GraphicsSplineGearAttributesWidget::
GraphicsSplineGearAttributesWidget(QWidget *parent) : GraphicsItemAttributesWidget(parent)
{
    m_splineAttributesWidget = new GraphicsSplineAttributesWidget(this);

    m_numberOfTeethSpinBox = new QSpinBox(this);
    m_numberOfTeethSpinBox->setMinimum(2);
    m_numberOfTeethLabel = new QLabel(tr("number of teeth"), this);
    m_numberOfTeethLabel->setBuddy(m_numberOfTeethSpinBox);
    connect(m_numberOfTeethSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeNumberOfTeeth(int)));


    m_radiusSlider = new RealValuedSlider("Radius", this);
    connect(m_radiusSlider, SIGNAL(valueChanged(real)), this, SLOT(changeRadius(real)));

    QWidget *gearWidget = new QWidget(this);
    QGridLayout *gearLayout = new QGridLayout(gearWidget);
    gearLayout->addWidget(m_numberOfTeethLabel    , 0, 0, 1, 1);
    gearLayout->addWidget(m_numberOfTeethSpinBox,   0, 1, 1, 1);
    gearLayout->addWidget(m_radiusSlider,           1, 0, 1, 2);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); //Sets border to 0, this results in same width as with only using the SplineAttributesWidget
    // layout->setSpacing(0); //uses no space between widgets in layout (not border)

    layout->addWidget(m_splineAttributesWidget);
    layout->addWidget(gearWidget);
    setLayout(layout);
}

GraphicsSplineGearAttributesWidget::
~GraphicsSplineGearAttributesWidget()
{}

void GraphicsSplineGearAttributesWidget::
setItem(QGraphicsItem *graphicsItem) {
    m_splineAttributesWidget->setItem(graphicsItem);
    m_currentGear = static_cast<GraphicsSplineGear*>(graphicsItem);
    updateAttributes();
}

bool GraphicsSplineGearAttributesWidget::
worksOnItem(QGraphicsItem *graphicsItem) {
    if(GraphicsSplineGear::isGraphicsSplineGearItem(graphicsItem))
        return true;
    return false;
}

void GraphicsSplineGearAttributesWidget::
updateAttributes() {
    m_numberOfTeethSpinBox->setValue(m_currentGear->numberOfTeeth());
    m_radiusSlider->setEnabled(true);
    m_radiusSlider->setRange(m_currentGear->minimumDistanceToCenter(), m_currentGear->maximumDistanceToCenter());
    m_radiusSlider->setValue(m_currentGear->radius());
}

void GraphicsSplineGearAttributesWidget::
changeNumberOfTeeth(int newToothCount) {
    if(newToothCount > 0)
        m_currentGear->setNumberOfTeeth(static_cast<uint>(newToothCount));
}

void GraphicsSplineGearAttributesWidget::
changeRadius(real newRadius) {
    m_currentGear->setRadius(newRadius);
}
