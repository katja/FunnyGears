#ifndef SPLINE_TOOTH_PROFILE
#define SPLINE_TOOTH_PROFILE

#include "basic_objects/Spline.h"
#include "basic_objects/ToothProfile.h"

class SplineToothProfile : public Spline, public ToothProfile {

public:
    SplineToothProfile(int degree = 2);
    SplineToothProfile(const Spline &other);
    virtual ~SplineToothProfile();
    SplineToothProfile* create() const override; // from ToothProfile
    SplineToothProfile* clone() const override; // from ToothProfile

    vec2 start() const override; // from ToothProfile
    vec2 stop() const override; // from ToothProfile
    real minimumDistanceToOrigin() const override; // from ToothProfile
    real maximumDistanceToOrigin() const override; // from ToothProfile
    void sampledCurve(vector<vec2> &samples) const override; // from ToothProfile

private:
    /** Searches for points in the spline curve whose normales pass the origin.
     *  Found points are necessary f.ex. for the calculation of the minimum and maximum
     *  distances of the curve to the origin.
     *  As resizing of the given @param samples vector should be prohibited, the samples
     *  vector must at least have the size of:
     *  2 x number of control points
     *  When a smaller samples vector is given, "-1" is returned.
     *  @param samples in this given vector are the points inserted whose normals pass the origin
     *  @return number of inserted points or -1 when given samples is too small
     */
    int pointsWithNormalsThroughOrigin(vector<vec2> &samples, real epsilon = 0.00001f) const;
    int testThroughOrigin(real evaluationValue, int &lastSign, int &foundPoints, vector<vec2> &samples, real epsilon) const;

};

#endif // SPLINE_TOOTH_PROFILE
