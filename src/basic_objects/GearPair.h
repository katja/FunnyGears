#ifndef GEAR_PAIR
#define GEAR_PAIR

#include "definitions.h"
#include "helpers.h"
#include "basic_objects/GearPairInformation.h"
#include "basic_objects/ContactPoint.h"
#include "basic_objects/ContactPointManager.h"
class SplineGear;


class GearPair {

public:
    GearPair(const SplineGear &drivingGear); //Driving gear = treibendes Rad
    ~GearPair();

    void calculateAgainWithAllAttributes();
    void calculateAgainWithNewToothProfile();
    void calculateAgainWithUnchangedAttributes();

    GearPairInformation* gearPairInformation();
    const ContactPointManager& contactPointManager();

    SplineGear* drivingGear() const;
    SplineGear* drivenGear() const;

    void setNumberOfTeethOfDrivenGear(uint numberOfTeeth);
    uint numberOfTeethOfDrivenGear() const;

    real module() const;
    real transmissionRatio() const;  //German: Übersetzung(sverhältnis)

    real drivingGearPitchRadius() const;
    real drivenGearPitchRadius() const;

    real getDistanceOfCenters() const;

    void setBottomClearance(real bottomClearance, real startAngleInDegree);
    real bottomClearance() const;
    real bottomClearanceStartAngle() const;

    void useBottomClearance(bool useIt);
    bool isBottomClearanceUsed() const;

    void setMaxDriftAngleInDegree(real degree);
    void setMaxDriftAngle(real rad);
    real maxDriftAngleInDegree() const;
    real maxDriftAngle() const; // in rad

    void setSamplingRate(uint samplingRate);
    uint samplingRate() const;

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
    // real m_drivenGearIndependentReferenceRadius;
    real m_module;
    Spline *m_completeToothProfile;

    bool m_useBottomClearance;
    real m_bottomClearance;
    real m_bottomClearanceStartAngle;

    real m_maxDriftAngle;
    uint m_samplingRate;
    real m_stepSize;

    GearPairInformation     *m_gearPairInformation;
    ContactPointManager m_contactPointManager;

    void insertPossiblePairingPointsInPointManager();
    void insertRefinedContactPoints(real stepValue, real nextStepValue, uint partition);
    void createAndInsertContactPoint(const vec2 &point, const vec2 &normal, real evalValue);
    ContactPoint* contactPointOf(const vec2 &point, const vec2 &normal, real evalValue, real t, bool usedLargerValue) const;
    NoneContactPoint* noneContactPointOf(const vec2 &point, const vec2 &normal, real stepValue) const;
    bool insertThicknessInContactPoint(ContactPoint& contactPoint) const;

    void fillDrivenGearWithGearPoints();
    void updateBottomClearanceTranslation();

    vec2 normalAt(real value) const;
};
#endif // GEAR_PAIR
