#ifndef GEAR_PAIR_INFORMATION_WIDGET
#define GEAR_PAIR_INFORMATION_WIDGET

#include "stable.h"
#include "basic_objects/GearPair.h"
#include "basic_objects/GearPairInformation.h"

class GearPairInformationWidget : public QWidget, public ChangingObjectListener {

public:
    GearPairInformationWidget(GearPair *gearPair);
    virtual ~GearPairInformationWidget();

    void updateInformation();

    void objectChanged(ChangingObject *object) override; // from ChangingObjectListener

protected:
    void showEvent(QShowEvent *event) override; // from QWidget
    void enterEvent(QEvent *event) override; // from QWidget
    void paintEvent(QPaintEvent *event) override; // from QWidget

private:
    GearPair            *m_gearPair;
    GearPairInformation *m_gearPairInfo;

    QLabel  //*m_validGear,
            *m_module,
            *m_drivingGearNumberOfTeeth,
            *m_drivenGearNumberOfTeeth,
            *m_transmissionRatio,
            *m_basicRequire,
            *m_basicFirstRequire,
            *m_basicSecondRequire;

};

#endif // GEAR_PAIR_INFORMATION_WIDGET
