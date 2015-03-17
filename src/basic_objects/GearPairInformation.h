#ifndef GEAR_PAIR_INFORMATION
#define GEAR_PAIR_INFORMATION

#include "definitions.h"
#include "basic_objects/GearPair.h"
#include "basic_objects/ContactPoint.h"
#include "basic_objects/ContactPointHelpers.h"
#include "basic_objects/ContactPointManager.h"
#include "helper_objects/ChangingObject.h"
#include "helper_objects/ChangingObjectListener.h"

class GearPairInformation : public ChangingObject, public ChangingObjectListener {

public:
    GearPairInformation(GearPair *gearPair);
    virtual ~GearPairInformation();

    // Savings of special characteristics in gear pair creation
    void update();

    // Retrieve information
    bool gearShapeOfDrivingGearIsValid() const; // false => Normals of the gear do not have a cut with the outline => outline has intersections ('<=>' is not correct! Outline may have intersections, but anyway every normal has cuttings with the gear shape!)
    bool gearOutlineConstructionOfDrivenGearFailed() const; // Algorithm of the mating gear construction failed. This may happen, if sampling is too vague => increase sampling rate and/or descrease max sampling angle

    bool basicRequirementsOfGearToothSystemAreFulfilled(TurningDirection directionOfDrivingGear) const; // Is true, if first and second basic requirement is fulfilled
    bool firstBasicRequirementOfGearToothSystemIsFulfilled(TurningDirection directionOfDrivingGear) const; // Is true, if for the used outline only valid ContactPoints are used (and gearShapeOfDrivingGearIsValid() and !gearOutlineConstructionOfDrivenGearFailed())
    bool secondBasicRequirementOfGearToothSystemIsFulfilled(TurningDirection directionOfDrivingGear) const; // Is true, if the path of contact covers a whole pitch angle

    real ratioOfCPsToWCPs(TurningDirection directionOfDrivingGear); // value between 0.0 and 1.0, if only CPs are used, 1.0 is returned, if none CPs are used, 0.0 is returned

    void objectChanged(ChangingObject *object) override; // from ChangingObjectListener
    void informAboutChange(ChangingObjectListener *listener) override; // from ChangingObject
    void noMoreInformAboutChange(ChangingObjectListener *listener) override; // from ChangingObject

private:
    GearPair *m_gearPair;
    const ContactPointManager *m_CPmanager;
    list<ChangingObjectListener*> m_listeners;

    bool             m_informationCalculated;
    Directions<bool> m_firstRequirement;
    Directions<uint> m_numberOfCPsInContact;
    Directions<uint> m_numberOfWCPsInContact;
    Directions<real> m_percentagePathOfContactCoversPitch;

    bool             m_BCinformationCalculated;
    Directions<bool> m_BCfirstRequirement;
    Directions<uint> m_BCnumberOfCPsInContact;
    Directions<uint> m_BCnumberOfWCPsInContact;
    Directions<real> m_BCpercentagePathOfContactCoversPitch;

    bool             m_updateNextTime;

    void changed(); //informs all current ChangingObjectListener to this object about changes
    void updateNecessary();
    void countDirection(vector<ContactPoint*> &contactPoints, Directions<uint> &numberOfCPsInContact) const;
    void countDirection(vector<WrongContactPoint*> &wrongContactPoints, Directions<uint> &numberOfCPsInContact) const;
    void updateFirstRequirement(Directions<bool> &firstRequirement, Directions<uint> numberOfWCPsInContact) const;
    void updateSecondRequirement(CalculationState state, Directions<real> &percentagePathOfContactCoversPitch) const;
};

#endif //GEAR_PAIR_INFORMATION
