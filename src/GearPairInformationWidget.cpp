#include "GearPairInformationWidget.h"

GearPairInformationWidget::GearPairInformationWidget(GearPair *gearPair) :
    m_gearPair(gearPair),
    m_gearPairInfo(gearPair->gearPairInformation())
{
    std::cout << "GearPairInformationWidget is created" << std::endl;
    setWindowTitle(tr("Gear Pair Information"));
    setMinimumHeight(150);
    setMinimumWidth(200);

    // QLabel *modulText = new QLabel(tr("Module"), this);
    m_validGear = new QLabel(this);
    m_module = new QLabel(this);
    // QLabel *drivingGearText = new QLabel(tr("Driving Gear"), this);
    // QLabel *drivenGearText = new QLabel(tr("Driven Gear"), this);
    // QLabel *numberOfTeethText = new QLabel(tr("Number of Teeth"), this);
    // m_drivingGearNumberOfTeeth = new QLabel(this);
    // m_drivenGearNumberOfTeeth = new QLabel(this);
    // QLabel *transmissionRatioText = new QLabel(tr("Transmission Ratio"), this);
    m_transmissionRatio = new QLabel(tr("i = ????"), this);
    // transmissionRatioText->setBuddy(m_transmissionRatio);
    m_transmissionRatio->setToolTip(tr("Mittlere Übersetzung i, durch feste Größen der Zähnezahlen bzw. Wälzkreisradien gegeben, berechnet durch: - z_b / z_a"));

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("Valid Driving Gear?"), m_validGear);
    formLayout->addRow(tr("Module"), m_module);
    formLayout->addRow(tr("Transmission Ratio"), m_transmissionRatio);
    setLayout(formLayout);

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
}

void GearPairInformationWidget::updateInformation() {
    m_validGear->setText((m_gearPairInfo->validGearShapeOfDrivingGear()) ? "Yes" : "No");
    m_module->setText("m = " + QString::number(m_gearPair->module(), 'f', 3));
    m_transmissionRatio->setText("i = " + QString::number(m_gearPair->transmissionRatio(), 'f', 3));
}

void GearPairInformationWidget::objectChanged(ChangingObject *object) {
    Q_UNUSED(object);
    std::cout << "objectChanged in gearPairInformation found" << std::endl;
    updateInformation();
}
