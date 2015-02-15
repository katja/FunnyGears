#ifndef GEAR_PAIR
#define GEAR_PAIR

#include "definitions.h"
#include "helpers.h"
#include "basic_objects/ContactPoint.h"
#include "basic_objects/ContactPointSortingList.h"
class SplineGear;


class GearPair {

public:
    GearPair(SplineGear *drivingGear); //Driving gear = treibendes Rad
    ~GearPair();

    std::list<ContactPoint>* foundPoints();
    Spline* completeToothProfile();
    std::list< PositionList* >* pointsInSortedLists();

    void setNumberOfTeethOfDrivenGear(uint numberOfTeeth);
    uint numberOfTeethOfDrivenGear() const;

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
    real m_drivenGearIndependentReferenceRadius;
    real m_module;
    Spline *m_completeToothProfile;

    real m_maxDriftAngle;
    uint m_samplingRate;
    real m_stepSize;
    bool m_liveCalculation;

    ContactPointSortingList m_allContactPoints;

    void constructListOfPossiblePairingPoints();
    void chooseCorrectPoints();
    ContactPoint contactPointOf(const vec2 &point, const vec2 &normal);
    vec2 normalAt(real value);
};
#endif // GEAR_PAIR
