#ifndef GEAR_PAIR_INFORMATION
#define GEAR_PAIR_INFORMATION

#include "definitions.h"
#include "basic_objects/GearPair.h"
#include "basic_objects/ContactPoint.h"
#include "basic_objects/ContactPointHelpers.h"
#include "basic_objects/ContactPointManager.h"
#include "helper_objects/ChangingObject.h"
#include "helper_objects/ChangingObjectListener.h"

/** Holds information about how good or bad the quality of the gearing is
 *
 *  Uses a GearPair and its ContactPointManager to get the informations.
 *  Therefore is initialized with a GearPair to work on.
 *
 *  Installs itself as a ChangingObjectListener to the GearPair. So when the GearPair
 *  changes (GearPair::informAboutChange()) it is informed with objectChanged(GearPair*)
 *
 *  Is at the same time a ChangingObject, too. This is necessary especially for the
 *  GearPairInformationWidget. As soon as the information of the GearPairInformation has
 *  changed, a corresponding GearPairInformationWidget should update its view
 *
 *  To avoid unnecessary calculations, the informations of a GearPairInformation object
 *  are only computed, if the corresponding GearPairInformationWidget is visible.
 *  Therefore the GearPairInformationWidget calls the update() on the GearPairInformation.
 *
 *  If used with a GearPairInformationWidget (normal case) the GearPairInformationWidget
 *  constructs a GearPairInformation and also deletes it again, when it is destroyed itself.
 */

class GearPairInformation : public ChangingObject, public ChangingObjectListener {

public:
    GearPairInformation(GearPair *gearPair);
    virtual ~GearPairInformation();

    void update();

    bool gearShapeOfDrivingGearIsValid() const; // false => Normals of the gear do not have a cut with the outline => outline has intersections ('<=>' is not correct! Outline may have intersections, but anyway every normal has cuttings with the gear shape!)
    bool gearOutlineConstructionOfDrivenGearFailed() const; // Algorithm of the mating gear construction failed. This may happen, if sampling is too vague => increase sampling rate and/or descrease max sampling angle

    bool basicRequirementsOfGearToothSystemAreFulfilled(TurningDirection directionOfDrivingGear) const; // Is true, if first and second basic requirement is fulfilled
    bool firstBasicRequirementOfGearToothSystemIsFulfilled(TurningDirection directionOfDrivingGear) const; // Is true, if for the used outline only valid ContactPoints are used (and gearShapeOfDrivingGearIsValid() and !gearOutlineConstructionOfDrivenGearFailed())
    bool secondBasicRequirementOfGearToothSystemIsFulfilled(TurningDirection directionOfDrivingGear) const; // Is true, if the path of contact covers a whole pitch angle

    real ratioOfCPsToWCPs(TurningDirection directionOfDrivingGear) const; // value between 0.0 and 1.0, if only CPs are used, 1.0 is returned, if none CPs are used, 0.0 is returned
    real contactRatio(TurningDirection directionOfDrivingGear) const; //German: Profilüberdeckung (epsilon_alpha) value between 0.0 and >1.0

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
