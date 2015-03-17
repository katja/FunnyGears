#include "GearPairInformationWidget.h"
#include "basic_objects/SplineGear.h"
#include "basic_objects/ContactPointHelpers.h"

GearPairInformationWidget::GearPairInformationWidget(GearPair *gearPair) :
    m_gearPair(gearPair),
    m_gearPairInfo(nullptr),
    m_attentionBox(nullptr),
    m_attentionLayout(nullptr),
    m_layout(nullptr),
    m_showInvalidGearShape(false),
    m_showIncorrectGearOutline(false)

{
    std::cout << "GearPairInformationWidget is created" << std::endl;
    setWindowTitle(tr("Gear Pair Information"));

    int minimumWidth = 160;
    int minimumHeight = 22;
    setMinimumWidth(3 * minimumWidth + 20);

    m_gearPairInfo = new GearPairInformation(m_gearPair);
    m_gearPairInfo->informAboutChange(this);

    //////////////////
    // Attention:
    m_invalidGearShape = new QLabel(this);
    m_invalidGearShape->setTextFormat(Qt::RichText);
    m_invalidGearShape->setText(tr(
        "<font color='#DC001E'><p><b>There were normals found in the driving gear with no cut with the gear outline.</b><br>This means, that the <b>outline has intersections!</b> The mating gear construction is not correct!</p></font>"));
    m_invalidGearShape->setMinimumWidth(3 * minimumWidth);
    m_invalidGearShape->setWordWrap(true);
    m_invalidGearShape->hide();
    m_incorrectGearOutline = new QLabel(this);
    m_incorrectGearOutline->setTextFormat(Qt::RichText);
    m_incorrectGearOutline->setText(tr(
        "<font color='#DC001E'><p><b>The algorithm for the construction of the mating gear failed!</b><br>This may happen, if <b>sampling is too vague</b> => so go to 'Rendering Setup' and increase the 'Sampling Rate' and/or decrease the 'Max. drift angle'.</p></font>"));
    m_incorrectGearOutline->setMinimumWidth(3 * minimumWidth);
    m_incorrectGearOutline->setWordWrap(true);
    m_incorrectGearOutline->hide();

    m_attentionBox = new QGroupBox(tr("Attention!"), this);
    m_attentionLayout = new QVBoxLayout(m_attentionBox);
    m_attentionLayout->setContentsMargins(4, 0, 4, 4);
    m_attentionLayout->setSpacing(3);
    m_attentionBox->hide();

    //////////////////
    // General:
    m_module = new QLabel(this);
    m_transmissionRatio = new QLabel(this);
    m_transmissionRatio->setToolTip(tr("Mittlere Übersetzung i, durch feste Größen der Zähnezahlen bzw. Wälzkreisradien gegeben, berechnet durch: - z_b / z_a"));

    QGroupBox *generalBox = new QGroupBox(tr("General"), this);
    QFormLayout *generalLayout = new QFormLayout(generalBox);
    generalLayout->setContentsMargins(4, 0, 4, 4);
    generalLayout->setSpacing(3);
    generalLayout->addRow(tr("Module"), m_module);
    generalLayout->addRow(tr("Transmission Ratio"), m_transmissionRatio);

    //////////////////
    // Comparison:
    QLabel *drivingGearText = new QLabel(tr("Driving Gear"), this);
    QLabel *drivenGearText = new QLabel(tr("Driven Gear"), this);

    QLabel *numberOfTeethText = new QLabel(tr("Number of Teeth"), this);
    numberOfTeethText->setWordWrap(true);
    m_drivingGearNumberOfTeeth = new QLabel(this);
    m_drivenGearNumberOfTeeth = new QLabel(this);
    numberOfTeethText->setBuddy(m_drivingGearNumberOfTeeth);

    QGroupBox *comparingBox = new QGroupBox(tr("Compare driving and driven gear"), this);
    QGridLayout *comparingLayout = new QGridLayout(comparingBox);
    comparingLayout->setContentsMargins(4, 0, 4, 4);
    comparingLayout->setHorizontalSpacing(6);
    comparingLayout->setVerticalSpacing(3);
    for(int column : {0, 1, 2}) {
        comparingLayout->setColumnMinimumWidth(column, minimumWidth);
    }

    int row = 0;
    comparingLayout->setRowMinimumHeight(row, minimumHeight);
    comparingLayout->addWidget(drivingGearText,             row, 1, 1, 1, Qt::AlignCenter);
    comparingLayout->addWidget(drivenGearText,              row, 2, 1, 1, Qt::AlignCenter);
    ++row;
    comparingLayout->setRowMinimumHeight(row, 2 * minimumHeight);
    comparingLayout->addWidget(numberOfTeethText,           row, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    comparingLayout->addWidget(m_drivingGearNumberOfTeeth,  row, 1, 1, 1, Qt::AlignCenter);
    comparingLayout->addWidget(m_drivenGearNumberOfTeeth,   row, 2, 1, 1, Qt::AlignCenter);
    ++row;
    comparingLayout->setRowStretch(                         row, 1);

    //////////////////
    // Quality:
    QLabel *clockwiseText = new QLabel(tr("Driving gear turns\nclockwise"), this);
    clockwiseText->setWordWrap(true);
    QLabel *counterClockwiseText = new QLabel(tr("Driving gear turns\ncounter clockwise"), this);
    counterClockwiseText->setWordWrap(true);

    QLabel *basicRequireText = new QLabel(tr("Basic requirements of gear tooth system are fulfilled?"), this);
    basicRequireText->setWordWrap(true);
    m_basicRequireC = new QLabel(this);
    m_basicRequireCC = new QLabel(this);

    QLabel *basicFirstRequireText = new QLabel(tr("First condition of basic requirements is fulfilled?"), this);
    basicFirstRequireText->setWordWrap(true);
    m_basicFirstRequireC = new QLabel(this);
    m_basicFirstRequireCC = new QLabel(this);

    QLabel *basicSecondRequireText = new QLabel(tr("Second condition of basic requirements is fulfilled?"), this);
    basicSecondRequireText->setWordWrap(true);
    m_basicSecondRequireC = new QLabel(this);
    m_basicSecondRequireCC = new QLabel(this);

    QGroupBox *qualityBox = new QGroupBox(tr("Quality of gearing"), this);
    QGridLayout *qualityLayout = new QGridLayout(qualityBox);
    qualityLayout->setContentsMargins(4, 0, 4, 4);
    qualityLayout->setHorizontalSpacing(6);
    qualityLayout->setVerticalSpacing(3);
    for(int column : {0, 1, 2}) {
        qualityLayout->setColumnMinimumWidth(column, minimumWidth);
    }

    row = 0;
    qualityLayout->setRowMinimumHeight(row, 2 * minimumHeight);
    qualityLayout->addWidget(clockwiseText,             row, 1, 1, 1, Qt::AlignTop | Qt::AlignHCenter);
    qualityLayout->addWidget(counterClockwiseText,      row, 2, 1, 1, Qt::AlignTop | Qt::AlignHCenter);
    ++row;
    qualityLayout->setRowMinimumHeight(row, 3 * minimumHeight);
    qualityLayout->addWidget(basicRequireText,          row, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    qualityLayout->addWidget(m_basicRequireC,           row, 1, 1, 1, Qt::AlignCenter);
    qualityLayout->addWidget(m_basicRequireCC,          row, 2, 1, 1, Qt::AlignCenter);
    ++row;
    qualityLayout->setRowMinimumHeight(row, 3 * minimumHeight);
    qualityLayout->addWidget(basicFirstRequireText,     row, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    qualityLayout->addWidget(m_basicFirstRequireC,      row, 1, 1, 1, Qt::AlignCenter);
    qualityLayout->addWidget(m_basicFirstRequireCC,     row, 2, 1, 1, Qt::AlignCenter);
    ++row;
    qualityLayout->setRowMinimumHeight(row, 3 * minimumHeight);
    qualityLayout->addWidget(basicSecondRequireText,    row, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    qualityLayout->addWidget(m_basicSecondRequireC,     row, 1, 1, 1, Qt::AlignCenter);
    qualityLayout->addWidget(m_basicSecondRequireCC,    row, 2, 1, 1, Qt::AlignCenter);
    ++row;
    qualityLayout->setRowStretch(                       row, 1);

    //////////////////
    // Put all together:
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(4, 0, 4, 4);
    m_layout->setSpacing(3);
    m_layout->addWidget(generalBox);
    m_layout->addWidget(comparingBox);
    m_layout->addWidget(qualityBox);
    m_layout->addStretch(1);

    updateInformation();
    hide();
}

GearPairInformationWidget::~GearPairInformationWidget() {
    std::cout << "GearPairInformationWidget is deleted" << std::endl;
    delete m_gearPairInfo;
}

void GearPairInformationWidget::updateInformation() {
    if(!isVisible())
        return;

    m_gearPairInfo->update();
    bool showInvalidGearShape = false;
    bool showIncorrectGearOutline = false;
    if(!m_gearPairInfo->gearShapeOfDrivingGearIsValid()) {
        showInvalidGearShape = true;
    }
    if(m_gearPairInfo->gearOutlineConstructionOfDrivenGearFailed()) {
        showIncorrectGearOutline = true;
    }
    if(showInvalidGearShape != m_showInvalidGearShape || showIncorrectGearOutline != m_showIncorrectGearOutline)
        updateAttentionBoxVisibility(showInvalidGearShape, showIncorrectGearOutline);

    m_module->setText("m = " + QString::number(m_gearPair->module(), 'f', 3) + " mm");
    m_transmissionRatio->setText("i = " + QString::number(m_gearPair->transmissionRatio(), 'f', 3));

    m_drivingGearNumberOfTeeth->setText("z = " + QString::number(m_gearPair->drivingGear()->numberOfTeeth()));
    m_drivenGearNumberOfTeeth->setText("z = " + QString::number(m_gearPair->numberOfTeethOfDrivenGear()));

    TurningDirection c = TurningDirection::Clockwise;
    TurningDirection cc = TurningDirection::CounterClockwise;

    m_basicRequireC->setText(
        (m_gearPairInfo->basicRequirementsOfGearToothSystemAreFulfilled(c)) ? "Yes" : "No");
    m_basicRequireCC->setText(
        (m_gearPairInfo->basicRequirementsOfGearToothSystemAreFulfilled(cc)) ? "Yes" : "No");
    m_basicFirstRequireC->setText(
        (m_gearPairInfo->firstBasicRequirementOfGearToothSystemIsFulfilled(c)) ? "Yes" : "No");
    m_basicFirstRequireCC->setText(
        (m_gearPairInfo->firstBasicRequirementOfGearToothSystemIsFulfilled(cc)) ? "Yes" : "No");
    m_basicSecondRequireC->setText(
        (m_gearPairInfo->secondBasicRequirementOfGearToothSystemIsFulfilled(c)) ? "Yes" : "No");
    m_basicSecondRequireCC->setText(
        (m_gearPairInfo->secondBasicRequirementOfGearToothSystemIsFulfilled(cc)) ? "Yes" : "No");

}

void GearPairInformationWidget::objectChanged(ChangingObject *object) {
    //received, when GraphicsGearPair changes (GraphicsGearPair creates this connection)
    //          or when GearPairInformation updated its status
    Q_UNUSED(object);
    updateInformation();
}

void GearPairInformationWidget::showEvent(QShowEvent *event) {
    updateInformation();
    QWidget::showEvent(event);
}

void GearPairInformationWidget::updateAttentionBoxVisibility(bool invalidGearShapeVisible, bool incorrectGearOutlineVisible) {
    if(m_showInvalidGearShape != invalidGearShapeVisible) {
        if(invalidGearShapeVisible) {
            m_attentionLayout->addWidget(m_invalidGearShape);
        } else {
            m_attentionLayout->removeWidget(m_invalidGearShape);
        }
        m_invalidGearShape->setVisible(invalidGearShapeVisible);
        m_showInvalidGearShape = invalidGearShapeVisible;
    }
    if(m_showIncorrectGearOutline != incorrectGearOutlineVisible) {
        if(incorrectGearOutlineVisible) {
            m_attentionLayout->addWidget(m_incorrectGearOutline);
        } else {
            m_attentionLayout->removeWidget(m_incorrectGearOutline);
        }
        m_incorrectGearOutline->setVisible(incorrectGearOutlineVisible);
        m_showIncorrectGearOutline = incorrectGearOutlineVisible;
    }
    if(m_showInvalidGearShape || m_showIncorrectGearOutline) {
        m_layout->addWidget(m_attentionBox);
        m_attentionBox->setVisible(true);
    } else {
        m_layout->removeWidget(m_attentionBox);
        m_attentionBox->setVisible(false);
    }
    if(height() < minimumSizeHint().height())
        resize(width(), minimumSizeHint().height());
}
