#ifndef GEAR_PAIR_INFORMATION
#define GEAR_PAIR_INFORMATION

#include "definitions.h"
#include "ChangingObject.h"
#include "ChangingObjectListener.h"
class GearPair;

class GearPairInformation : public ChangingObject, public ChangingObjectListener {

public:
    GearPairInformation(GearPair *gearPair);
    virtual ~GearPairInformation();

    // Savings of special characteristics in gear pair creation
    void setBackAttributes();
    void notValidGearShapeFound(); // called when normals of the gear do not have a cut with the outline => outline has intersections ('<=>' is not correct! Outline may have intersections, but anyway every normal has cuttings with the gear shape!)
    void matingGearOutlineConstructionFailed(); // called when algorithm of the mating gear construction failed. This may happen, if sampling is too vague => increase sampling rate and/or descrease max sampling angle

    // Retrieve information
    real approximatePercentageOfInContactPoints() const;
    bool validGearShapeOfDrivingGear() const;

    void objectChanged(ChangingObject *object) override; // from ChangingObjectListener
    void informAboutChange(ChangingObjectListener *listener) override; // from ChangingObject
    void noMoreInformAboutChange(ChangingObjectListener *listener) override; // from ChangingObject
    void changed(); //informs all current ChangingObjectListener to this object about changes

private:
    GearPair *m_gearPair;
    bool m_gearShapeValid;
    bool m_gearConstructionValid;
    list<ChangingObjectListener*> m_listeners;
};

#endif //GEAR_PAIR_INFORMATION
