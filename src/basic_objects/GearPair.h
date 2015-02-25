#ifndef GEAR_PAIR
#define GEAR_PAIR

#include "definitions.h"
#include "helpers.h"
#include "basic_objects/ContactPoint.h"
#include "basic_objects/ContactPointSortingList.h"
class SplineGear;


class GearPair {

public:
    GearPair(const SplineGear &drivingGear); //Driving gear = treibendes Rad
    ~GearPair();

    void updateDrivingGearChange();

    void doCalculation();

    const ContactPointSortingList& foundPoints();
    Spline* completeToothProfile();
    const std::list<PointsWithPosition*>& pointsInSortedLists();
    const std::list<Triangle>& triangles();
    const ContactPoint& startPoint() const;
    const std::vector< NoneContactPoint* >& noneContactPoints() const;
    vector<vec2> gearPoints() const;

    SplineGear* drivingGear() const;
    SplineGear* drivenGear() const;

    void setNumberOfTeethOfDrivenGear(uint numberOfTeeth);
    uint numberOfTeethOfDrivenGear() const;

    real transmissionRatio() const;  //German: Übersetzung(sverhältnis)

    real drivingGearPitchRadius() const;
    real drivenGearPitchRadius() const;

    real getDistanceOfCenters() const;

    void setMaxDriftAngleInDegree(real degree);
    void setMaxDriftAngle(real rad);
    real maxDriftAngleInDegree() const;
    real maxDriftAngle() const; // in rad

    void setSamplingRate(uint samplingRate);
    uint samplingRate() const;

private:
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

    std::vector<NoneContactPoint*> m_noneContactPoints;

    real m_maxDriftAngle;
    uint m_samplingRate;
    real m_stepSize;

    ContactPointSortingList m_allContactPoints;

    void constructListOfPossiblePairingPoints();
    void insertRefinedContactPoints(real stepValue, real nextStepValue, uint partition);
    void chooseCorrectPoints();

    ContactPoint* contactPointOf(const vec2 &point, const vec2 &normal, real stepValue);
    NoneContactPoint* convertToNoneContactPoint(ContactPoint *contactPoint);
    void insertThicknessInContactPoint(ContactPoint& contactPoint) const;
    vec2 normalAt(real value) const;
};
#endif // GEAR_PAIR
