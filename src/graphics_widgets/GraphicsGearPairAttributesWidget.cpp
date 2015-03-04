#include "GraphicsGearPairAttributesWidget.h"

GraphicsGearPairAttributesWidget::GraphicsGearPairAttributesWidget(QWidget *parent) :
    GraphicsItemAttributesWidget(parent)
{
    m_moduleLabel = new QLabel(this);
    QLabel *moduleLabelText = new QLabel(tr("Module"), this);
    moduleLabelText->setBuddy(m_moduleLabel);

    m_toothCountDrivenGearSpinBox = new QSpinBox(this);
    m_toothCountDrivenGearSpinBox->setMinimum(2);
    connect(m_toothCountDrivenGearSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeNumberOfTeethOfDrivenGear(int)));
    QLabel *toothCountLabel = new QLabel(tr("Number of teeth of driven gear"), this);
    toothCountLabel->setBuddy(m_toothCountDrivenGearSpinBox);

    m_drivingGearEnabledCheckBox = new QCheckBox(tr("Enable editing (driving gear)"), this);
    connect(m_drivingGearEnabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleDrivingGearEnabled(bool)));

    m_drivingGearSamplingCheckBox = new QCheckBox(tr("...driving gear sampling"), this);
    connect(m_drivingGearSamplingCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleDrivingGearSamplingVisibility(bool)));

    m_drivingGearForbiddenAreaCheckBox = new QCheckBox(tr("...forbidden area (driving gear)"), this);
    connect(m_drivingGearForbiddenAreaCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleForbiddenAreaInDrivingGear(bool)));

    m_drivenGearSamplingCheckBox = new QCheckBox(tr("...driven gear sampling"), this);
    connect(m_drivenGearSamplingCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleDrivenGearSamplingVisibility(bool)));

    m_drivenGearForbiddenAreaCheckBox = new QCheckBox(tr("...forbidden area (driven gear)"), this);
    connect(m_drivenGearForbiddenAreaCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleForbiddenAreaInDrivenGear(bool)));

    m_noneContactPointsCheckBox = new QCheckBox(tr("...paths of points without contact"), this);
    connect(m_noneContactPointsCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleNoneContactPointsVisibility(bool)));

    m_selectedGearPointsCheckBox = new QCheckBox(tr("...selected points"), this);
    connect(m_selectedGearPointsCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleSelectedGearPointsVisibility(bool)));

    m_pathOfPossibleContactCheckBox = new QCheckBox(tr("...path of possible contact"), this);
    connect(m_pathOfPossibleContactCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePathOfPossibleContactVisibility(bool)));

    m_pathOfRealContactCheckBox = new QCheckBox(tr("...path of real contact"), this);
    connect(m_pathOfRealContactCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePathOfRealContactVisibility(bool)));

    m_pitchesCheckBox = new QCheckBox(tr("...pitches"), this);
    connect(m_pitchesCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePitchesVisibility(bool)));

    m_pitchCirclesCheckBox = new QCheckBox(tr("...pitch circles"), this);
    connect(m_pitchCirclesCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePitchCirclesVisibility(bool)));

    QGroupBox *showBox = new QGroupBox(tr("Show..."), this);
    QVBoxLayout *showBoxLayout = new QVBoxLayout(showBox);
    showBoxLayout->addWidget(m_drivingGearSamplingCheckBox);
    showBoxLayout->addWidget(m_drivenGearSamplingCheckBox);
    showBoxLayout->addWidget(m_drivingGearForbiddenAreaCheckBox);
    showBoxLayout->addWidget(m_drivenGearForbiddenAreaCheckBox);
    showBoxLayout->addWidget(m_noneContactPointsCheckBox);
    showBoxLayout->addWidget(m_selectedGearPointsCheckBox);
    showBoxLayout->addWidget(m_pathOfPossibleContactCheckBox);
    showBoxLayout->addWidget(m_pathOfRealContactCheckBox);
    showBoxLayout->addWidget(m_pitchesCheckBox);
    showBoxLayout->addWidget(m_pitchCirclesCheckBox);

    m_rotationCheckBox = new QCheckBox(tr("Rotate the gears?"), this);
    connect(m_rotationCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleRotationOfGears(bool)));
    m_rotationVelocitySlider = new RealValuedSlider("Rotation velocity", this);
    m_rotationVelocitySlider->setRange(-2.0, 2.0);
    m_rotationVelocitySlider->setEnabled(false);
    connect(m_rotationVelocitySlider, SIGNAL(valueChanged(real)), this, SLOT(changeRotationVelocity(real)));

    m_samplingRateSpinBox = new QSpinBox(this);
    m_samplingRateSpinBox->setRange(10, 150);
    connect(m_samplingRateSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeSamplingRate(int)));
    QLabel *samplingRateLabel = new QLabel(tr("Sampling Rate"), this);
    samplingRateLabel->setBuddy(m_samplingRateSpinBox);

    m_maxDriftAngleSpinBox = new QSpinBox(this);
    m_maxDriftAngleSpinBox->setRange(1, 180);
    m_maxDriftAngleSpinBox->setSuffix("°");
    connect(m_maxDriftAngleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeMaxDriftAngle(int)));
    QLabel *maxDriftAngleLabel = new QLabel(tr("Max. drift angle"), this);
    maxDriftAngleLabel->setBuddy(m_maxDriftAngleSpinBox);

    m_smallPencilWidthCheckBox = new QCheckBox(tr("Use fine pencil for drawing"), this);
    connect(m_smallPencilWidthCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePencilWidth(bool)));

    QGroupBox *renderingSetup = new QGroupBox(tr("Rendering setup"), this);
    QGridLayout *renderingSetupLayout = new QGridLayout(renderingSetup);
    renderingSetupLayout->addWidget(samplingRateLabel,             0, 0);
    renderingSetupLayout->addWidget(m_samplingRateSpinBox,         0, 1);
    renderingSetupLayout->addWidget(maxDriftAngleLabel,            1, 0);
    renderingSetupLayout->addWidget(m_maxDriftAngleSpinBox,        1, 1);
    renderingSetupLayout->addWidget(m_smallPencilWidthCheckBox,    2, 0, 1, 2);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(moduleLabelText,               0, 0);
    layout->addWidget(m_moduleLabel,                 0, 1);
    layout->addWidget(toothCountLabel,               1, 0);
    layout->addWidget(m_toothCountDrivenGearSpinBox, 1, 1);
    layout->addWidget(m_drivingGearEnabledCheckBox,  2, 0, 1, 1);
    layout->addWidget(showBox,                       3, 0, 1, 2);
    layout->addWidget(m_rotationCheckBox,            4, 0, 1, 2);
    layout->addWidget(m_rotationVelocitySlider,      5, 0, 1, 2);
    layout->addWidget(renderingSetup,                6, 0, 1, 2);

    layout->setRowStretch(7, 1); //stretch the empty space at bottom and not _between_ the objects
}

GraphicsGearPairAttributesWidget::~GraphicsGearPairAttributesWidget() {
}

void GraphicsGearPairAttributesWidget::updateAttributes() {
    m_moduleLabel->setText(QString::number(m_currentGearPair->module(), 'f', 3) + " mm");
    m_toothCountDrivenGearSpinBox->setValue(m_currentGearPair->numberOfTeethOfDrivenGear());
    m_drivingGearEnabledCheckBox->setChecked(m_currentGearPair->isDrivingGearEnabled());

    m_drivingGearSamplingCheckBox->setChecked(m_currentGearPair->visibilityOfDrivingGearSampling());
    m_drivingGearForbiddenAreaCheckBox->setChecked(m_currentGearPair->visibilityOfForbiddenAreaInDrivingGear());
    if(m_drivingGearSamplingCheckBox->isChecked())
        m_drivingGearForbiddenAreaCheckBox->setEnabled(true);
    else
        m_drivingGearForbiddenAreaCheckBox->setEnabled(false);

    m_drivenGearSamplingCheckBox->setChecked(m_currentGearPair->visibilityOfDrivenGearSampling());
    m_drivenGearForbiddenAreaCheckBox->setChecked(m_currentGearPair->visibilityOfForbiddenAreaInDrivenGear());
    if(m_drivenGearSamplingCheckBox->isChecked())
        m_drivenGearForbiddenAreaCheckBox->setEnabled(true);
    else
        m_drivenGearForbiddenAreaCheckBox->setEnabled(false);

    m_noneContactPointsCheckBox->setChecked(m_currentGearPair->visibilityOfNoneContactPoints());
    m_selectedGearPointsCheckBox->setChecked(m_currentGearPair->visibilityOfSelectedGearPoints());
    m_pathOfPossibleContactCheckBox->setChecked(m_currentGearPair->visibilityOfPathOfPossibleContact());
    m_pathOfRealContactCheckBox->setChecked(m_currentGearPair->visibilityOfPathOfRealContact());
    m_pitchesCheckBox->setChecked(m_currentGearPair->visibilityOfPitches());
    m_pitchCirclesCheckBox->setChecked(m_currentGearPair->visibilityOfPitchCircles());

    m_rotationCheckBox->setChecked(m_currentGearPair->isRotating());
    m_rotationVelocitySlider->setValue(m_currentGearPair->rotationVelocity());
    if(m_rotationCheckBox->isChecked())
        m_rotationVelocitySlider->setEnabled(true);
    else
        m_rotationVelocitySlider->setEnabled(false);

    m_samplingRate = m_currentGearPair->samplingRate();
    m_samplingRateSpinBox->setValue(m_samplingRate);
    m_maxDriftAngle = m_currentGearPair->maxDriftAngleInDegree();
    m_maxDriftAngleSpinBox->setValue(m_maxDriftAngle);

    m_smallPencilWidthCheckBox->setChecked(m_currentGearPair->finePencilUsed());
}

void GraphicsGearPairAttributesWidget::objectChanged(ChangingObject *object) {
    if(m_currentGearPair == object) {
        m_currentGearPair->update();
        updateAttributes();
    }
}

void GraphicsGearPairAttributesWidget::setItem(QGraphicsItem *graphicsItem) {
    m_currentGearPair = static_cast<GraphicsGearPair*>(graphicsItem);
    updateAttributes();
    m_currentGearPair->informAboutChange(this);
}

bool GraphicsGearPairAttributesWidget::worksOnItem(QGraphicsItem *graphicsItem) {
    if(GraphicsGearPair::isGraphicsGearPairItem(graphicsItem))
        return true;
    return false;
}

void GraphicsGearPairAttributesWidget::toggleDrivingGearEnabled(bool checked) {
    m_currentGearPair->setDrivingGearEnabled(checked);
}

void GraphicsGearPairAttributesWidget::changeNumberOfTeethOfDrivenGear(int newToothCount) {
    if(newToothCount > 0 && m_currentGearPair->numberOfTeethOfDrivenGear() != (uint)newToothCount)
        m_currentGearPair->setNumberOfTeethOfDrivenGear(static_cast<uint>(newToothCount));
}

void GraphicsGearPairAttributesWidget::toggleDrivingGearSamplingVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfDrivingGearSampling(checked);
    updateAttributes();
}

void GraphicsGearPairAttributesWidget::toggleForbiddenAreaInDrivingGear(bool checked) {
    m_currentGearPair->setVisibilityOfForbiddenAreaInDrivingGear(checked);
}

void GraphicsGearPairAttributesWidget::toggleDrivenGearSamplingVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfDrivenGearSampling(checked);
    updateAttributes();
}

void GraphicsGearPairAttributesWidget::toggleForbiddenAreaInDrivenGear(bool checked) {
    m_currentGearPair->setVisibilityOfForbiddenAreaInDrivenGear(checked);
}

void GraphicsGearPairAttributesWidget::toggleNoneContactPointsVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfNoneContactPoints(checked);
}

void GraphicsGearPairAttributesWidget::toggleSelectedGearPointsVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfSelectedGearPoints(checked);
}

void GraphicsGearPairAttributesWidget::togglePathOfPossibleContactVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfPathOfPossibleContact(checked);
}

void GraphicsGearPairAttributesWidget::togglePathOfRealContactVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfPathOfRealContact(checked);
}

void GraphicsGearPairAttributesWidget::togglePitchesVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfPitches(checked);
}

void GraphicsGearPairAttributesWidget::togglePitchCirclesVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfPitchCircles(checked);
}

void GraphicsGearPairAttributesWidget::toggleRotationOfGears(bool checked) {
    m_currentGearPair->setRotating(checked);
    updateAttributes();
}

void GraphicsGearPairAttributesWidget::changeRotationVelocity(real velocity) {
    m_currentGearPair->setRotationVelocity(velocity);
}

void GraphicsGearPairAttributesWidget::changeSamplingRate(int samplingRate) {
    if(m_samplingRate == samplingRate) //only update the gear pair, when samplingRate really changed!
        return;
    m_samplingRate = samplingRate;
    m_currentGearPair->setSamplingRate(m_samplingRate);
}

void GraphicsGearPairAttributesWidget::changeMaxDriftAngle(int maxDriftAngle) {
    if(m_maxDriftAngle == maxDriftAngle) //only update the gear pair, when maxDriftAngle really changed!
        return;
    m_maxDriftAngle = maxDriftAngle;
    m_currentGearPair->setMaxDriftAngleInDegree(m_maxDriftAngle);
}

void GraphicsGearPairAttributesWidget::togglePencilWidth(bool isFinePencil) {
    m_currentGearPair->setFinePencil(isFinePencil);
}
