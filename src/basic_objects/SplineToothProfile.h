#ifndef SPLINE_TOOTH_PROFILE
#define SPLINE_TOOTH_PROFILE

#include "Spline.h"
#include "ToothProfile.h"

class SplineToothProfile : public Spline, public ToothProfile {

public:
    SplineToothProfile(int degree = 2);
    SplineToothProfile(const Spline &other);
    virtual ~SplineToothProfile();
    SplineToothProfile* create() const;
    SplineToothProfile* clone() const;

    vec2 start() const;
    real getMinimumDistanceToOrigin() const;
    real getMaximumDistanceToOrigin() const;
    void sampledCurve(vector<vec2> &samples) const;

private:
    /** Searches for points in the spline curve whose normales pass the origin
     *  As resizing of the samples vector should be prohibited, the samples vector must
     *  at least have the size of:
     *  2 x number of control points
     *  When a smaller samples vector is given, "-1" is returned.
     *  @param samples in this given vector are the points inserted whose normals pass the origin
     *  @return number of inserted points or -1 when given samples is too small
     */
    int pointsWithNormalsThroughOrigin(vector<vec2> &samples, real epsilon = 0.00001f) const;
    int testThroughOrigin(real evaluationValue, int &lastSign, int &foundPoints, vector<vec2> &samples, real epsilon) const;

};

#endif // SPLINE_TOOTH_PROFILE
