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

    m_pitchVisibleCheckBox = new QCheckBox(tr("Show base pitch"), this);
    connect(m_pitchVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePitchVisiblity(bool)));

    m_pitchCircleVisibleCheckBox = new QCheckBox(tr("Show pitch circle"), this);
    connect(m_pitchCircleVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePitchCircleVisibility(bool)));

    m_animateCheckbox = new QCheckBox(tr("Rotate the gear?"), this);
    connect(m_animateCheckbox, SIGNAL(toggled(bool)), this, SLOT(toggleAnimation(bool)));
    m_rotationVelocitySlider = new RealValuedSlider("Rotation velocity", this);
    m_rotationVelocitySlider->setRange(-2.0f, 2.0f);
    connect(m_rotationVelocitySlider, SIGNAL(valueChanged(real)), this, SLOT(changeRotationVelocity(real)));

    QWidget *gearWidget = new QWidget(this);
    QGridLayout *gearLayout = new QGridLayout(gearWidget);
    gearLayout->addWidget(m_numberOfTeethLabel,         0, 0, 1, 1);
    gearLayout->addWidget(m_numberOfTeethSpinBox,       0, 1, 1, 1);
    gearLayout->addWidget(m_radiusSlider,               1, 0, 1, 2);
    gearLayout->addWidget(m_pitchVisibleCheckBox,       2, 0, 1, 1);
    gearLayout->addWidget(m_pitchCircleVisibleCheckBox, 2, 1, 1, 1);
    gearLayout->addWidget(m_animateCheckbox,            3, 0, 1, 1);
    gearLayout->addWidget(m_rotationVelocitySlider,     3, 1, 1, 1);

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
    m_pitchVisibleCheckBox->setChecked(m_currentGear->isBasePitchVisible());
    m_pitchCircleVisibleCheckBox->setChecked(m_currentGear->isPitchCircleVisible());
    m_animateCheckbox->setChecked(m_currentGear->isRotating());
    m_rotationVelocitySlider->setValue(m_currentGear->rotationVelocity());
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

void GraphicsSplineGearAttributesWidget::
togglePitchVisiblity(bool isVisible) {
    m_currentGear->setBasePitchVisibility(isVisible);
}

void GraphicsSplineGearAttributesWidget::
togglePitchCircleVisibility(bool isVisible) {
    m_currentGear->setPitchCircleVisibility(isVisible);
}

void GraphicsSplineGearAttributesWidget::
toggleAnimation(bool isAnimated) {
    m_currentGear->setRotating(isAnimated);
}

void GraphicsSplineGearAttributesWidget::
changeRotationVelocity(real velocity) {
    m_currentGear->setRotationVelocity(velocity);
}
