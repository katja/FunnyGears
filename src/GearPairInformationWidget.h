#ifndef GEAR_PAIR_INFORMATION_WIDGET
#define GEAR_PAIR_INFORMATION_WIDGET

#include "stable.h"
#include "basic_objects/GearPair.h"

class GearPairInformationWidget : public QWidget, public ChangingObjectListener {

public:
    GearPairInformationWidget(GearPair *gearPair);
    virtual ~GearPairInformationWidget();

    void updateInformation();

    void objectChanged(ChangingObject *object) override; // from ChangingObjectListener



private:
    GearPair            *m_gearPair;
    GearPairInformation *m_gearPairInfo;

    QLabel  *m_validGear,
            *m_module,
            *m_drivingGearNumberOfTeeth,
            *m_drivenGearNumberOfTeeth,
            *m_transmissionRatio;

};

#endif // GEAR_PAIR_INFORMATION_WIDGET
