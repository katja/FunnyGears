#include "GraphicsGearPairAttributesWidget.h"

GraphicsGearPairAttributesWidget::GraphicsGearPairAttributesWidget(QWidget *parent) :
    GraphicsItemAttributesWidget(parent)
{

    m_sampledGearToothCheckBox = new QCheckBox(tr("Show sampled gear tooth"), this);
    connect(m_sampledGearToothCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleSampledGearToothVisibility(bool)));

    m_noneContactPointsCheckBox = new QCheckBox(tr("Show paths of points without contact"), this);
    connect(m_noneContactPointsCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleNoneContactPointsVisibility(bool)));

    m_drivingGearWidthSamplingCheckBox = new QCheckBox(tr("Show sampling of width in driving gear"), this);
    connect(m_drivingGearWidthSamplingCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleSamplingWidthVisibilityInDrivingGear(bool)));

    m_drivenGearForbiddenAreaCheckBox = new QCheckBox(tr("Show normals for forbidden area"), this);
    connect(m_drivenGearForbiddenAreaCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleForbiddenAreaInDrivenGear(bool)));

    m_filledForbiddenAreaCheckBox = new QCheckBox(tr("Show created filled forbidden area"), this);
    connect(m_filledForbiddenAreaCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleFilledForbiddenAreaVisibility(bool)));

    m_pathOfContactCheckBox = new QCheckBox(tr("Show path of contact"), this);
    connect(m_pathOfContactCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePathOfContactVisibility(bool)));

    m_pitchesCheckBox = new QCheckBox(tr("Show pitches"), this);
    connect(m_pitchesCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePitchesVisibility(bool)));

    m_pitchCirclesCheckBox = new QCheckBox(tr("Show pitch circles"), this);
    connect(m_pitchCirclesCheckBox, SIGNAL(toggled(bool)), this, SLOT(togglePitchCirclesVisibility(bool)));

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

    m_isLiveUpdating = false;
    m_liveUpdatingCheckBox = new QCheckBox(tr("Update gear pair on changes"), this);
    connect(m_liveUpdatingCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleLiveUpdating(bool)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_sampledGearToothCheckBox);
    layout->addWidget(m_noneContactPointsCheckBox);
    layout->addWidget(m_drivingGearWidthSamplingCheckBox);
    layout->addWidget(m_drivenGearForbiddenAreaCheckBox);
    layout->addWidget(m_filledForbiddenAreaCheckBox);
    layout->addWidget(m_pathOfContactCheckBox);
    layout->addWidget(m_pitchesCheckBox);
    layout->addWidget(m_pitchCirclesCheckBox);
    layout->addWidget(m_rotationCheckBox);
    layout->addWidget(m_rotationVelocitySlider);
    layout->addWidget(samplingRateLabel);
    layout->addWidget(m_samplingRateSpinBox);
    layout->addWidget(maxDriftAngleLabel);
    layout->addWidget(m_maxDriftAngleSpinBox);
    layout->addWidget(m_liveUpdatingCheckBox);

    layout->addStretch(1); //stretch the empty space at bottom and not _between_ the objects
}

GraphicsGearPairAttributesWidget::~GraphicsGearPairAttributesWidget() {
}

void GraphicsGearPairAttributesWidget::updateAttributes() {
    m_sampledGearToothCheckBox->setChecked(m_currentGearPair->visibilityOfSampledGearTooth());
    m_noneContactPointsCheckBox->setChecked(m_currentGearPair->visibilityOfNoneContactPoints());
    m_drivingGearWidthSamplingCheckBox->setChecked(m_currentGearPair->visibilityOfSamplingWidthInDrivingGear());
    m_drivenGearForbiddenAreaCheckBox->setChecked(m_currentGearPair->visibilityOfForbiddenArea());
    m_filledForbiddenAreaCheckBox->setChecked(m_currentGearPair->visibilityOfFilledForbiddenArea());
    m_pathOfContactCheckBox->setChecked(m_currentGearPair->visibilityOfPathOfContact());
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
    m_liveUpdatingCheckBox->setChecked(m_isLiveUpdating);
}

void GraphicsGearPairAttributesWidget::objectChanged(ChangingObject *object) {
    if(m_currentGearPair == object) {
        if(m_isLiveUpdating)
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
    if(GraphicsGearPair::isGraphicsGearPair(graphicsItem))
        return true;
    return false;
}

void GraphicsGearPairAttributesWidget::toggleSampledGearToothVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfSampledGearTooth(checked);
}

void GraphicsGearPairAttributesWidget::toggleNoneContactPointsVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfNoneContactPoints(checked);
}

void GraphicsGearPairAttributesWidget::toggleSamplingWidthVisibilityInDrivingGear(bool checked) {
    m_currentGearPair->setVisibilityOfSamplingWidthInDrivingGear(checked);
}

void GraphicsGearPairAttributesWidget::toggleForbiddenAreaInDrivenGear(bool checked) {
    m_currentGearPair->setVisibilityOfForbiddenArea(checked);
}

void GraphicsGearPairAttributesWidget::toggleFilledForbiddenAreaVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfFilledForbiddenArea(checked);
}

void GraphicsGearPairAttributesWidget::togglePathOfContactVisibility(bool checked) {
    m_currentGearPair->setVisibilityOfPathOfContact(checked);
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

void GraphicsGearPairAttributesWidget::toggleLiveUpdating(bool checked) {
    m_isLiveUpdating = checked;
}
