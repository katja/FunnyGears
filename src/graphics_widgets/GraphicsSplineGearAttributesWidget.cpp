#include "graphics_widgets/GraphicsSplineGearAttributesWidget.h"
#include "basic_objects/SplineGear.h"

GraphicsSplineGearAttributesWidget::
GraphicsSplineGearAttributesWidget(QWidget *parent) : GraphicsItemAttributesWidget(parent)
{
    m_splineAttributesWidget = new GraphicsSplineAttributesWidget(this);
    m_splineAttributesWidget->layout()->setContentsMargins(0, 0, 0, 0); //Set border to 0, as with this the attributes group boxes of spline and gear can have same width

    m_moduleSpinBox = new QDoubleSpinBox(this);
    m_moduleSpinBox->setDecimals(3);
    m_moduleSpinBox->setMinimum(0.005);
    m_moduleSpinBox->setMaximum(1000.0);
    m_moduleSpinBox->setSingleStep(0.005);
    m_moduleLabel = new QLabel(tr("Module"), this);
    m_moduleLabel->setBuddy(m_moduleSpinBox);
    connect(m_moduleSpinBox, SIGNAL(editingFinished()), this, SLOT(updateModule()));

    m_numberOfTeethSpinBox = new QSpinBox(this);
    m_numberOfTeethSpinBox->setMinimum(2);
    m_numberOfTeethLabel = new QLabel(tr("Number of teeth"), this);
    m_numberOfTeethLabel->setBuddy(m_numberOfTeethSpinBox);
    connect(m_numberOfTeethSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeNumberOfTeeth(int)));

    m_referenceRadiusSlider = new RealValuedSlider("Reference circle radius", this);
    connect(m_referenceRadiusSlider, SIGNAL(valueChanged(real)), this, SLOT(changeRadius(real)));

    m_pitchVisibleCheckBox = new QCheckBox(tr("Show base pitch"), this);
    connect(m_pitchVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePitchVisiblity(bool)));

    m_referenceCircleVisibleCheckBox = new QCheckBox(tr("Show pitch circle"), this);
    connect(m_referenceCircleVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleReferenceCircleVisibility(bool)));

    m_animateCheckbox = new QCheckBox(tr("Rotate the gear?"), this);
    connect(m_animateCheckbox, SIGNAL(toggled(bool)), this, SLOT(toggleAnimation(bool)));
    m_rotationVelocitySlider = new RealValuedSlider("Rotation velocity", this);
    m_rotationVelocitySlider->setRange(-2.0, 2.0);
    connect(m_rotationVelocitySlider, SIGNAL(valueChanged(real)), this, SLOT(changeRotationVelocity(real)));

    QGroupBox *gearWidget = new QGroupBox(tr("Gear Attributes"), this);

    QGridLayout *gearLayout = new QGridLayout(gearWidget);
    gearLayout->setContentsMargins(4, 0, 12, 12);
    gearLayout->addWidget(m_moduleLabel,                0, 0, 1, 1);
    gearLayout->addWidget(m_moduleSpinBox,              0, 1, 1, 1);
    gearLayout->addWidget(m_numberOfTeethLabel,         1, 0, 1, 1);
    gearLayout->addWidget(m_numberOfTeethSpinBox,       1, 1, 1, 1);
    gearLayout->addWidget(m_referenceRadiusSlider,               2, 0, 1, 2);
    gearLayout->addWidget(m_pitchVisibleCheckBox,       3, 0, 1, 1);
    gearLayout->addWidget(m_referenceCircleVisibleCheckBox, 3, 1, 1, 1);
    gearLayout->addWidget(m_animateCheckbox,            4, 0, 1, 1);
    gearLayout->addWidget(m_rotationVelocitySlider,     4, 1, 1, 1);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 0, 4, 4); //use small margins at left, right and bottom
    // layout->setSpacing(0); //uses no space between widgets in layout (not border)

    layout->addWidget(m_splineAttributesWidget);
    layout->addWidget(gearWidget);
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
    m_moduleSpinBox->setValue(m_currentGear->module());
    m_numberOfTeethSpinBox->setValue(m_currentGear->numberOfTeeth());
    m_referenceRadiusSlider->setRange(m_currentGear->minimumDistanceToCenter(), m_currentGear->maximumDistanceToCenter());
    m_referenceRadiusSlider->setValue(m_currentGear->referenceRadius());
    m_pitchVisibleCheckBox->setChecked(m_currentGear->isBasePitchVisible());
    m_referenceCircleVisibleCheckBox->setChecked(m_currentGear->isReferenceCircleVisible());
    m_animateCheckbox->setChecked(m_currentGear->isRotating());
    m_rotationVelocitySlider->setValue(m_currentGear->rotationVelocity());
}

void GraphicsSplineGearAttributesWidget::
updateModule() {
    real module = m_moduleSpinBox->value();
    if(m_currentGear->module() != module) {
        m_currentGear->setModule(module);
        std::cout << "before updateAttributes" << std::endl;
        updateAttributes();
        std::cout << "after  updateAttributes" << std::endl;
    }
}

void GraphicsSplineGearAttributesWidget::
changeNumberOfTeeth(int newToothCount) {
    if(newToothCount > 0 && m_currentGear->numberOfTeeth() != (uint)newToothCount) {
        m_currentGear->setNumberOfTeeth(static_cast<uint>(newToothCount));
        updateAttributes();
    }
}

void GraphicsSplineGearAttributesWidget::
changeRadius(real newRadius) {
    if(m_currentGear->referenceRadius() != newRadius) {
        m_currentGear->setReferenceRadius(newRadius);
        updateAttributes();
    }
}

void GraphicsSplineGearAttributesWidget::
togglePitchVisiblity(bool isVisible) {
    m_currentGear->setBasePitchVisibility(isVisible);
}

void GraphicsSplineGearAttributesWidget::
toggleReferenceCircleVisibility(bool isVisible) {
    m_currentGear->setReferenceCircleVisibility(isVisible);
}

void GraphicsSplineGearAttributesWidget::
toggleAnimation(bool isAnimated) {
    m_currentGear->setRotating(isAnimated);
}

void GraphicsSplineGearAttributesWidget::
changeRotationVelocity(real velocity) {
    m_currentGear->setRotationVelocity(velocity);
}