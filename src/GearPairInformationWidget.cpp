#include "GearPairInformationWidget.h"
#include "basic_objects/SplineGear.h"
#include "basic_objects/ContactPointHelpers.h"

GearPairInformationWidget::GearPairInformationWidget(GearPair *gearPair) :
    m_gearPair(gearPair),
    m_gearPairInfo(nullptr)
{
    std::cout << "GearPairInformationWidget is created" << std::endl;
    setWindowTitle(tr("Gear Pair Information"));
    setMinimumHeight(150);
    setMinimumWidth(200);

    m_gearPairInfo = new GearPairInformation(m_gearPair);

    // m_validGear = new QLabel(this);

    //////////////////
    // General:
    m_module = new QLabel(this);
    m_transmissionRatio = new QLabel(this);
    m_transmissionRatio->setToolTip(tr("Mittlere Übersetzung i, durch feste Größen der Zähnezahlen bzw. Wälzkreisradien gegeben, berechnet durch: - z_b / z_a"));

    QGroupBox *generalBox = new QGroupBox(tr("General"), this);
    QFormLayout *generalLayout = new QFormLayout(generalBox);
    generalLayout->setContentsMargins(4, 0, 4, 4);
    generalLayout->addRow(tr("Module"), m_module);
    generalLayout->addRow(tr("Transmission Ratio"), m_transmissionRatio);

    //////////////////
    // Comparison:
    QLabel *drivingGearText = new QLabel(tr("Driving Gear"), this);
    QLabel *drivenGearText = new QLabel(tr("Driven Gear"), this);

    QLabel *numberOfTeethText = new QLabel(tr("Number of Teeth"), this);
    m_drivingGearNumberOfTeeth = new QLabel(this);
    m_drivenGearNumberOfTeeth = new QLabel(this);
    numberOfTeethText->setBuddy(m_drivingGearNumberOfTeeth);

    QGroupBox *comparingBox = new QGroupBox(tr("Compare driving and driven gear"), this);
    QGridLayout *comparingLayout = new QGridLayout(comparingBox);
    comparingLayout->setContentsMargins(4, 0, 4, 4);

    comparingLayout->addWidget(drivingGearText,   0, 1, 1, 1);
    comparingLayout->addWidget(drivenGearText,    0, 2, 1, 1);
    comparingLayout->addWidget(numberOfTeethText, 1, 0, 1, 1);
    comparingLayout->addWidget(m_drivingGearNumberOfTeeth, 1, 1, 1, 1);
    comparingLayout->addWidget(m_drivenGearNumberOfTeeth,  1, 2, 1, 1);


    //////////////////
    // Quality:

    QLabel *basicRequireText = new QLabel(tr("Basic requirements of gear tooth system are fulfilled?"), this);
    m_basicRequire = new QLabel(this);
    QLabel *basicFirstRequireText = new QLabel(tr("First condition of basic requirements is fulfilled?"), this);
    m_basicFirstRequire = new QLabel(this);
    QLabel *basicSecondRequireText = new QLabel(tr("Second condition of basic requirements is fulfilled?"), this);
    m_basicSecondRequire = new QLabel(this);

    QGroupBox *qualityBox = new QGroupBox(tr("Quality of gearing"), this);
    QVBoxLayout *qualityLayout = new QVBoxLayout(qualityBox);
    qualityLayout->addWidget(basicRequireText);
    qualityLayout->addWidget(m_basicRequire);
    qualityLayout->addWidget(basicFirstRequireText);
    qualityLayout->addWidget(m_basicFirstRequire);
    qualityLayout->addWidget(basicSecondRequireText);
    qualityLayout->addWidget(m_basicSecondRequire);

    //////////////////
    // Put all together:
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 0, 4, 4);
    layout->addWidget(generalBox);
    layout->addWidget(comparingBox);
    layout->addWidget(qualityBox);

    updateInformation();
    hide();

    //Modul
    //Zähnezahl 1 vs Zähnezahl 2
    //Übersetzung i
    //1. Verzahnungsgesetz erfüllt? Für Punkte in Kontakt durch Konstruktion praktisch immer, aber sind sie in Kontakt???
    //2. Verzhanungsgesetz erfüllt?
    //Informationen dazu

}

GearPairInformationWidget::~GearPairInformationWidget() {
    std::cout << "GearPairInformationWidget is deleted" << std::endl;
    delete m_gearPairInfo;
}

void GearPairInformationWidget::updateInformation() {
    m_gearPairInfo->update();
    // m_validGear->setText((m_gearPairInfo->gearShapeOfDrivingGearIsValid()) ? "Yes" : "No");
    m_module->setText("m = " + QString::number(m_gearPair->module(), 'f', 3) + " mm");
    m_transmissionRatio->setText("i = " + QString::number(m_gearPair->transmissionRatio(), 'f', 3));
    m_drivingGearNumberOfTeeth->setText("z = " + QString::number(m_gearPair->drivingGear()->numberOfTeeth()));
    m_drivenGearNumberOfTeeth->setText("z = " + QString::number(m_gearPair->numberOfTeethOfDrivenGear()));
    m_basicRequire->setText((m_gearPairInfo->basicRequirementsOfGearToothSystemAreFulfilled(TurningDirection::Clockwise)) ? "Yes" : "No");
    m_basicFirstRequire->setText((m_gearPairInfo->firstBasicRequirementOfGearToothSystemIsFulfilled(TurningDirection::Clockwise)) ? "Yes" : "No");
    m_basicSecondRequire->setText((m_gearPairInfo->secondBasicRequirementOfGearToothSystemIsFulfilled(TurningDirection::Clockwise)) ? "Yes" : "No");
}

void GearPairInformationWidget::objectChanged(ChangingObject *object) {
    //received, when GraphicsGearPair changes (GraphicsGearPair creates this connection)
    Q_UNUSED(object);
    updateInformation();
}

void GearPairInformationWidget::showEvent(QShowEvent *event) {
    updateInformation();
    QWidget::showEvent(event);
}

void GearPairInformationWidget::enterEvent(QEvent *event) {
    updateInformation();
    QWidget::enterEvent(event);
}

void GearPairInformationWidget::paintEvent(QPaintEvent *event) {
    updateInformation();
    QWidget::paintEvent(event);
}
