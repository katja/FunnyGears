#include "GraphicsGearPairAttributesWidget.h"

GraphicsGearPairAttributesWidget::GraphicsGearPairAttributesWidget(QWidget *parent) :
    GraphicsItemAttributesWidget(parent)
{

    //////////////////
    //Information:

    m_moduleLabel = new QLabel(this);
    QLabel *moduleLabelText = new QLabel(tr("Module:"), this);
    moduleLabelText->setBuddy(m_moduleLabel);

    QPushButton *gearPairInformationButton = new QPushButton(tr("More ..."), this);
    connect(gearPairInformationButton, SIGNAL(clicked()), this, SLOT(showGearPairInformation()));

    QGroupBox *informationBox = new QGroupBox(tr("Information"), this);
    QHBoxLayout *informationLayout = new QHBoxLayout(informationBox);
    informationLayout->setContentsMargins(4, 0, 4, 4);
    informationLayout->setSpacing(3);
    informationLayout->addWidget(moduleLabelText);
    informationLayout->addWidget(m_moduleLabel);
    informationLayout->addStretch();
    informationLayout->addWidget(gearPairInformationButton);

    //////////////////
    //Gearing Adaption:

    m_toothCountDrivenGearSpinBox = new QSpinBox(this);
    m_toothCountDrivenGearSpinBox->setMinimum(2);
    connect(m_toothCountDrivenGearSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeNumberOfTeethOfDrivenGear(int)));
    QLabel *toothCountLabel = new QLabel(tr("Number of teeth of driven gear"), this);
    toothCountLabel->setBuddy(m_toothCountDrivenGearSpinBox);

    m_drivingGearEnabledCheckBox = new QCheckBox(tr("Enable editing (driving gear)"), this);
    connect(m_drivingGearEnabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleDrivingGearEnabled(bool)));

    m_useBottomClearanceCheckBox = new QCheckBox(tr("Use bottom clearance"), this);
    connect(m_useBottomClearanceCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleUseBottomClearance(bool)));

    m_bottomClearanceSpinBox = new QSpinBox(this);
    m_bottomClearanceSpinBox->setMinimum(0);
    m_bottomClearanceSpinBox->setSuffix(" mm");
    connect(m_bottomClearanceSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeBottomClearance(int)));
    QLabel *bottomClearanceLabel = new QLabel(tr("Bottom clearance"), this);
    bottomClearanceLabel->setBuddy(m_bottomClearanceSpinBox);

    m_bottomClearanceStartAngleSpinBox = new QSpinBox(this);
    m_bottomClearanceStartAngleSpinBox->setRange(2, 45);
    m_bottomClearanceStartAngleSpinBox->setSuffix("°");
    connect(m_bottomClearanceStartAngleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeBottomClearanceStartAngle(int)));
    QLabel *bottomClearanceStartAngleLabel = new QLabel(tr("Start angle"), this);
    bottomClearanceStartAngleLabel->setBuddy(m_bottomClearanceStartAngleSpinBox);

    QGroupBox *bottomClearanceBox = new QGroupBox(tr("Bottom clearance"), this);
    QGridLayout *bottomClearanceLayout = new QGridLayout(bottomClearanceBox);
    bottomClearanceLayout->setContentsMargins(4, 0, 4, 4);
    bottomClearanceLayout->setHorizontalSpacing(6);
    bottomClearanceLayout->setVerticalSpacing(3);
    bottomClearanceLayout->addWidget(m_useBottomClearanceCheckBox,          0, 0, 1, 2);
    bottomClearanceLayout->addWidget(bottomClearanceLabel,                  1, 0);
    bottomClearanceLayout->addWidget(m_bottomClearanceSpinBox,              1, 1);
    bottomClearanceLayout->addWidget(bottomClearanceStartAngleLabel,        2, 0);
    bottomClearanceLayout->addWidget(m_bottomClearanceStartAngleSpinBox,    2, 1);

    QGroupBox *gearingAdaptionBox = new QGroupBox(tr("Adaption of the gearing"));
    QGridLayout *gearingAdaptionLayout = new QGridLayout(gearingAdaptionBox);
    gearingAdaptionLayout->setContentsMargins(4, 0, 4, 4);
    gearingAdaptionLayout->setHorizontalSpacing(6);
    gearingAdaptionLayout->setVerticalSpacing(3);
    gearingAdaptionLayout->addWidget(toothCountLabel,               0, 0, Qt::AlignLeft);
    gearingAdaptionLayout->addWidget(m_toothCountDrivenGearSpinBox, 0, 1, Qt::AlignRight);
    gearingAdaptionLayout->addWidget(m_drivingGearEnabledCheckBox,  1, 0, 1, 2);
    gearingAdaptionLayout->addWidget(bottomClearanceBox,            2, 0, 1, 2);
    gearingAdaptionLayout->setRowStretch(3, 1);

    //////////////////
    //Show...:

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
    showBoxLayout->setContentsMargins(4, 0, 4, 4);
    showBoxLayout->setSpacing(3);
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

    //////////////////
    //Rotation:

    m_rotationCheckBox = new QCheckBox(tr("Rotate the gears?"), this);
    connect(m_rotationCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleRotationOfGears(bool)));
    m_rotationVelocitySlider = new RealValuedSlider("Rotation velocity", this);
    m_rotationVelocitySlider->setRange(-2.0, 2.0);
    m_rotationVelocitySlider->setEnabled(false);
    connect(m_rotationVelocitySlider, SIGNAL(valueChanged(real)), this, SLOT(changeRotationVelocity(real)));

    QGroupBox *rotationBox = new QGroupBox(tr("Rotation"), this);
    QVBoxLayout *rotationLayout = new QVBoxLayout(rotationBox);
    rotationLayout->setContentsMargins(4, 0, 4, 4);
    rotationLayout->setSpacing(3);
    rotationLayout->addWidget(m_rotationCheckBox);
    rotationLayout->addWidget(m_rotationVelocitySlider);
    rotationLayout->addStretch(1);

    //////////////////
    //Rendering Setup:

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

    QGroupBox *renderingSetupBox = new QGroupBox(tr("Rendering setup"));
    QGridLayout *renderingSetupLayout = new QGridLayout(renderingSetupBox);
    renderingSetupLayout->setContentsMargins(0, 0, 0, 0);
    renderingSetupLayout->setHorizontalSpacing(3);
    renderingSetupLayout->setVerticalSpacing(3);
    renderingSetupLayout->addWidget(samplingRateLabel,             0, 0);
    renderingSetupLayout->addWidget(m_samplingRateSpinBox,         0, 1);
    renderingSetupLayout->addWidget(maxDriftAngleLabel,            1, 0);
    renderingSetupLayout->addWidget(m_maxDriftAngleSpinBox,        1, 1);
    renderingSetupLayout->addWidget(m_smallPencilWidthCheckBox,    2, 0, 1, 2);
    renderingSetupLayout->setRowStretch(3, 1);

    ////////////////////////////////
    //... SET ALL TOGETHER...

    QTabWidget *tab = new QTabWidget(this);
    tab->addTab(gearingAdaptionBox, tr("Adaption"));
    tab->addTab(rotationBox, tr("Rotation"));
    tab->addTab(renderingSetupBox, tr("Rendering setup"));

    QGroupBox *gearPairWidget = new QGroupBox(tr("Gear Pairing Attributes"), this);
    QVBoxLayout *gearPairLayout = new QVBoxLayout(gearPairWidget);
    gearPairLayout->setContentsMargins(4, 0, 4, 4);
    gearPairLayout->setSpacing(15);
    gearPairLayout->addWidget(informationBox);
    gearPairLayout->addWidget(tab);
    gearPairLayout->addWidget(showBox);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 0, 4, 4); //use small margins at left, right and bottom (left, top, right, bottom)
    layout->setSpacing(0);
    layout->addWidget(gearPairWidget);
    layout->addStretch(); //stretch the empty space at bottom and not _between_ the objects
}

GraphicsGearPairAttributesWidget::~GraphicsGearPairAttributesWidget() {
}

void GraphicsGearPairAttributesWidget::updateAttributes() {
    m_moduleLabel->setText(QString::number(m_currentGearPair->module(), 'f', 3) + " mm");
    m_toothCountDrivenGearSpinBox->setValue(m_currentGearPair->numberOfTeethOfDrivenGear());
    m_drivingGearEnabledCheckBox->setChecked(m_currentGearPair->isDrivingGearEnabled());

    m_useBottomClearanceCheckBox->setChecked(m_currentGearPair->isBottomClearanceUsed());
    m_bottomClearance = m_currentGearPair->bottomClearance();
    m_bottomClearanceStartAngle = m_currentGearPair->bottomClearanceStartAngle();
    m_bottomClearanceSpinBox->setValue(m_bottomClearance);
    m_bottomClearanceSpinBox->setEnabled(m_useBottomClearanceCheckBox->isChecked());
    m_bottomClearanceStartAngleSpinBox->setValue(m_bottomClearanceStartAngle);
    m_bottomClearanceStartAngleSpinBox->setEnabled(m_useBottomClearanceCheckBox->isChecked());

    m_drivingGearSamplingCheckBox->setChecked(m_currentGearPair->visibilityOfDrivingGearSampling());
    m_drivingGearForbiddenAreaCheckBox->setChecked(m_currentGearPair->visibilityOfForbiddenAreaInDrivingGear());
    m_drivingGearForbiddenAreaCheckBox->setEnabled(m_drivingGearSamplingCheckBox->isChecked());

    m_drivenGearSamplingCheckBox->setChecked(m_currentGearPair->visibilityOfDrivenGearSampling());
    m_drivenGearForbiddenAreaCheckBox->setChecked(m_currentGearPair->visibilityOfForbiddenAreaInDrivenGear());
    m_drivenGearForbiddenAreaCheckBox->setEnabled(m_drivenGearSamplingCheckBox->isChecked());

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

void GraphicsGearPairAttributesWidget::toggleUseBottomClearance(bool checked) {
    m_currentGearPair->useBottomClearance(checked);
    updateAttributes();
}

void GraphicsGearPairAttributesWidget::changeBottomClearance(int bottomClearance) {
    if(m_bottomClearance == bottomClearance)
        return;
    m_bottomClearance = bottomClearance;
    m_currentGearPair->setBottomClearance(m_bottomClearance, m_bottomClearanceStartAngle);
}

void GraphicsGearPairAttributesWidget::changeBottomClearanceStartAngle(int startAngle) {
    if(m_bottomClearanceStartAngle == startAngle)
        return;
    m_bottomClearanceStartAngle = startAngle;
    m_currentGearPair->setBottomClearance(m_bottomClearance, m_bottomClearanceStartAngle);
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

void GraphicsGearPairAttributesWidget::showGearPairInformation() {
    m_currentGearPair->showGearPairInformationWidget();
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
