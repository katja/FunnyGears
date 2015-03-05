#ifndef GEAR_PAIR_INFORMATION
#define GEAR_PAIR_INFORMATION

#include "definitions.h"
#include "ChangingObject.h"
#include "ChangingObjectListener.h"
class GearPair;

class GearPairInformation : public ChangingObject {

public:
    GearPairInformation(GearPair *gearPair);
    virtual ~GearPairInformation();

    // Savings of special characteristics in gear pair creation
    void setBackAttributes();
    void notValidGearShapeFound();


    // Retrieve information
    real approximatePercentageOfInContactPoints() const;
    bool validGearShapeOfDrivingGear() const;


    void informAboutChange(ChangingObjectListener *listener) override; // from ChangingObject
    void noMoreInformAboutChange(ChangingObjectListener *listener) override; // from ChangingObject
    void changed();

private:
    GearPair *m_gearPair;
    bool m_gearShapeValid;
    std::list<ChangingObjectListener*> m_listeners;
};

#endif //GEAR_PAIR_INFORMATION
