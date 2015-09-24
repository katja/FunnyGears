#ifndef GEAR_PAIR
#define GEAR_PAIR

#include "definitions.h"
#include "helpers.h"
#include "basic_objects/ContactPoint.h"
#include "basic_objects/ContactPointManager.h"
#include "helper_objects/ChangingObject.h"
#include "helper_objects/ChangingObjectListener.h"
class SplineGear;

/** Represents a pair of gears
 *
 *  The given SplineGear drivingGear is copied and the class holds every time a reference
 *  of drivingGear and drivenGear. So it is always possible to set f. ex. the numberOfTeeth
 *  even if the SplineGears (drivingGear/drivenGear) are not valid (!isValid())
 *  But most other methods rely on the validness. See the discription at each method.
 */
class GearPair : public ChangingObject {

public:
    GearPair(const SplineGear &drivingGear); //Driving gear = treibendes Rad
    virtual ~GearPair();

    /** Complete update of the GearPair
     *  Calculates m_drivingGearPitchRadius, m_module, m_drivenGearPitchRadius and
     *  m_distanceOfCenters and continues the update with calculateAgainWithNewToothProfile()
     */
    void calculateAgainWithAllAttributes();

    /** Updates the m_completeToothProfile, the main quadrant...
     *  Checks validness (isValid()) and if it does not succeed, calls only
     *  calculateAgainWithUnchangedAttributes().
     *  Otherwise the former m_completeToothProfile is deleted and a new one
     *  calculated out of the current m_drivingGear. The main quadrant is set again and
     *  the update continues with calculateAgainWithUnchangedAttributes()
     */
    void calculateAgainWithNewToothProfile();

    /** Updates all points of driven gear
     *  Deletes information of m_contactPointManager, calculates sampling interval and
     *  then calculates pairing points and therefore the drivne gear
     *  If !isValid() only the state of m_contactPointManager is set back and changed()
     *  is called
     */
    void calculateAgainWithUnchangedAttributes();


    const ContactPointManager& contactPointManager(); //should be possible with !isValid()

    SplineGear* drivingGear() const; //possible with !isValid()
    SplineGear* drivenGear() const; //possible with !isValid()

    void setNumberOfTeethOfDrivenGear(uint numberOfTeeth); //possible with !isValid()
    uint numberOfTeethOfDrivenGear() const; //possible with !isValid()

    real module() const; //possible with !isValid() (but doesn't make sense)
    real transmissionRatio() const;  //German: Übersetzung(sverhältnis)  //possible with !isValid() (if driving gear has 0 teeth, 0 is returned)

    real drivingGearPitchRadius() const; //possible with !isValid() (but doesn't make sense)
    real drivenGearPitchRadius() const; //possible with !isValid() (but doesn't make sense)

    real getDistanceOfCenters() const; //possible with !isValid() (but doesn't make sense)

    void setBottomClearance(real bottomClearance, real startAngleInDegree);  //possible with !isValid()
    real bottomClearance() const; //possible with !isValid()
    real bottomClearanceStartAngle() const; //possible with !isValid()

    void useBottomClearance(bool useIt); //possible with !isValid() (only saves this setting, does no recalculation)
    bool isBottomClearanceUsed() const;  //possible with !isValid()

    void setMaxDriftAngleInDegree(real degree); //possible with !isValid()
    void setMaxDriftAngle(real rad); //possible with !isValid()
    real maxDriftAngleInDegree() const; //possible with !isValid()
    real maxDriftAngle() const; // in rad, possible with !isValid()

    void setSamplingRate(uint samplingRate); //possible with !isValid()
    uint samplingRate() const; //possible with !isValid()

    void informAboutChange(ChangingObjectListener *listener) override; // from ChangingObject, possible with !isValid()
    void noMoreInformAboutChange(ChangingObjectListener *listener) override; // from ChangingObject, possible with !isValid()

    bool isValid() const; //possible with !isValid()

private:
    static const real DefaultBottomClearance;
    static const real DefaultBottomClearanceStartAngle;
    static const real DefaultMaxDrift;
    static const uint DefaultSamplingRate;

    SplineGear *m_drivingGear;
    SplineGear *m_drivenGear;
    real m_drivingGearPitchRadius;
    real m_drivenGearPitchRadius;
    real m_distanceOfCenters;
    real m_module;
    Spline *m_completeToothProfile;
    int m_quadrantOfToothProfile; //estimation in which quadrant the main of the complete tooth profile is positioned (is necessary for correct turning angles)

    bool m_useBottomClearance;
    real m_bottomClearance;
    real m_bottomClearanceStartAngle; //in degree

    real m_maxDriftAngle; //in rad
    uint m_samplingRate;
    real m_stepSize;

    ContactPointManager m_contactPointManager;

    list<ChangingObjectListener*> m_listeners;

    void changed(); //possible with !isValid()

    void updateMainQuadrant(vec2 middleOfToothProfile); //method on its own is possible with !isValid(), but one should calculate the middleOfToothProfile before, which is only possible when isValid()

    void insertPossiblePairingPointsInPointManager(); //only possible when isValid()
    void insertRefinedContactPoints(real stepValue, real nextStepValue, uint partition); //only possible when isValid()
    void createAndInsertContactPoint(const vec2 &point, const vec2 &normal, real evalValue); //method on its own is possible with !isValid(), but one should calculate the middleOfToothProfile before, which is only possible when isValid()
    ContactPoint* contactPointOf(const vec2 &point, const vec2 &normal, real evalValue, real t, bool usedLargerValue) const; //method on its own is possible with !isValid(), but one should calculate the middleOfToothProfile before, which is only possible when isValid()
    NoneContactPoint* noneContactPointOf(const vec2 &point, const vec2 &normal, real stepValue) const; //method on its own is possible with !isValid(), but one should calculate the middleOfToothProfile before, which is only possible when isValid()
    bool insertThicknessInContactPoint(ContactPoint& contactPoint) const; //method on its own is possible with !isValid(), but one should calculate the middleOfToothProfile before, which is only possible when isValid()

    void fillDrivenGearWithGearPoints(); //only possible when isValid()
    void updateBottomClearanceTranslation(); //only possible when isValid()

    vec2 normalAt(real value) const; //MUST NOT BE CALLED, when !isValid(). Validness is not checked here!!!
};
#endif // GEAR_PAIR
