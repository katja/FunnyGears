#include "graphics_widgets/GraphicsSplineGearAttributesWidget.h"
#include "basic_objects/SplineGear.h"
#include "helpers.h"

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

    m_referenceRadiusSpinBox = new QDoubleSpinBox(this);
    m_referenceRadiusSpinBox->setDecimals(2);
    m_referenceRadiusSpinBox->setMinimum(5.0);
    m_referenceRadiusSpinBox->setMaximum(500.0);
    m_referenceRadiusSpinBox->setSingleStep(1.0);
    m_referenceRadiusLabel = new QLabel(tr("Reference radius"), this);
    m_referenceRadiusLabel->setBuddy(m_referenceRadiusSpinBox);
    connect(m_referenceRadiusSpinBox, SIGNAL(editingFinished()), this, SLOT(updateRadius()));

    QGroupBox *radiusModuleBox = new QGroupBox(tr("Depend on each other"), this);
    QGridLayout *radiusModuleLayout = new QGridLayout(radiusModuleBox);
    radiusModuleLayout->addWidget(m_moduleLabel,            0, 0, 1, 1);
    radiusModuleLayout->addWidget(m_moduleSpinBox,          0, 1, 1, 1);
    radiusModuleLayout->addWidget(m_referenceRadiusLabel,   1, 0, 1, 1);
    radiusModuleLayout->addWidget(m_referenceRadiusSpinBox, 1, 1, 1, 1);

    m_numberOfTeethSpinBox = new QSpinBox(this);
    m_numberOfTeethSpinBox->setMinimum(2);
    m_numberOfTeethLabel = new QLabel(tr("Number of teeth"), this);
    m_numberOfTeethLabel->setBuddy(m_numberOfTeethSpinBox);
    connect(m_numberOfTeethSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeNumberOfTeeth(int)));

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
    gearLayout->addWidget(radiusModuleBox,              0, 0, 1, 2);
    gearLayout->addWidget(m_numberOfTeethLabel,         1, 0);
    gearLayout->addWidget(m_numberOfTeethSpinBox,       1, 1);
    gearLayout->addWidget(m_pitchVisibleCheckBox,       3, 0);
    gearLayout->addWidget(m_referenceCircleVisibleCheckBox, 3, 1);
    gearLayout->addWidget(m_animateCheckbox,            4, 0);
    gearLayout->addWidget(m_rotationVelocitySlider,     4, 1);
    gearLayout->setRowStretch(5, 1); // As row 5 is not filled with a widget, the layout fills with empty space, if it is larger than needed

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
    m_currentGear->informAboutChange(this);
}

bool GraphicsSplineGearAttributesWidget::
worksOnItem(QGraphicsItem *graphicsItem) {
    if(GraphicsSplineGear::isGraphicsSplineGearItem(graphicsItem))
        return true;
    return false;
}

void GraphicsSplineGearAttributesWidget::
objectChanged(ChangingObject *object) {
    if(object == m_currentGear)
        updateAttributes();
}

void GraphicsSplineGearAttributesWidget::
updateAttributes() {
    m_moduleSpinBox->setValue(m_currentGear->module());
    m_referenceRadiusSpinBox->setValue(m_currentGear->referenceRadius());
    m_numberOfTeethSpinBox->setValue(m_currentGear->numberOfTeeth());
    m_pitchVisibleCheckBox->setChecked(m_currentGear->isBasePitchVisible());
    m_referenceCircleVisibleCheckBox->setChecked(m_currentGear->isReferenceCircleVisible());
    m_animateCheckbox->setChecked(m_currentGear->isRotating());
    m_rotationVelocitySlider->setValue(m_currentGear->rotationVelocity());
}

void GraphicsSplineGearAttributesWidget::
updateModule() {
    real module = m_moduleSpinBox->value();
    if(absolute(m_currentGear->module() - module) > Epsilon) {
        m_currentGear->setModule(module);
        updateAttributes();
    }
}

void GraphicsSplineGearAttributesWidget::
updateRadius() {
    real radius = m_referenceRadiusSpinBox->value();
    if(absolute(m_currentGear->referenceRadius() - radius) > Epsilon) {
        m_currentGear->setReferenceRadius(radius);
        updateAttributes();
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

const real GraphicsSplineGearAttributesWidget::Epsilon = 0.0001;
