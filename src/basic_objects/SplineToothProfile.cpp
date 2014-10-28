#include "SplineToothProfile.h"
#include "helpers.h"
#include <cmath>
#include <Eigen/LU>

SplineToothProfile::SplineToothProfile(int degree) : Spline(degree) {
    std::cout << "SplineToothProfile normal constructor" << std::endl;
}

SplineToothProfile::SplineToothProfile(const Spline &other) : Spline(other) {
    std::cout << "SplineToothProfile copy constructor" << std::endl;
}

SplineToothProfile::~SplineToothProfile() {
    std::cout << "SplineToothProfile destructor" << std::endl;
}

SplineToothProfile* SplineToothProfile::create() const {
    return new SplineToothProfile();
}

SplineToothProfile* SplineToothProfile::clone() const {
    return new SplineToothProfile(*this);
}


vec2 SplineToothProfile::start() const {
    return evaluate(lowerDomainLimit());
}

real SplineToothProfile::getMinimumDistanceToOrigin() const {
    vector<vec2> possiblePoints(2 * m_controlPoints.size());
    int numOfPossiblePoints = pointsWithNormalsThroughOrigin(possiblePoints);
    real minDistance = possiblePoints[0].norm();
    for(int i = 1; i < numOfPossiblePoints; ++i) {
        if(possiblePoints[i].norm() < minDistance)
            minDistance = possiblePoints[i].norm();
    }
    return minDistance;
}

real SplineToothProfile::getMaximumDistanceToOrigin() const {
    vector<vec2> possiblePoints(2 * m_controlPoints.size());
    int numOfPossiblePoints = pointsWithNormalsThroughOrigin(possiblePoints);
    real maxDistance = possiblePoints[0].norm();
    for(int i = 1; i < numOfPossiblePoints; ++i) {
        if(possiblePoints[i].norm() > maxDistance)
            maxDistance = possiblePoints[i].norm();
    }
    return maxDistance;
}

void SplineToothProfile::sampledCurve(vector<vec2> &samples) const {
    //TODO: is this method necessary???
}

int SplineToothProfile::pointsWithNormalsThroughOrigin(vector<vec2> &samples, real epsilon) const {
    if((samples.size() < 2 * m_controlPoints.size()) || !isValid())
        return -1;

    //Preparation of the sampleValues vector, which holds evaluable knots and one knot value between
    uint evaluableKnots = m_controlPoints.size() + 1 - m_degree;
    vector<real> sampleValues(evaluableKnots * 2 - 1);
    //insert all evaluable knots at every second position of sampleValues
    for(uint i = 0; i < evaluableKnots; ++i) {
        sampleValues[2 * i] = m_knots[i + m_degree];
    }
    //insert an additional value between knot values
    for(uint i = 1; i < sampleValues.size(); i += 2) {
        sampleValues[i] = (sampleValues[i - 1] + sampleValues[i + 1]) / 2.0f;
    }
    sampleValues[sampleValues.size() - 1] = sampleValues.back() - epsilon;

    //Now search for all points of the spline curve whose normals pass the origin
    //This is the case, when the cross product of the vector of the normal with
    //the vector to the respective point is "0". But most times one won't simply find these
    //points at the values of the sampleValues vector. Therefore the important thing is the
    //sign of the cross product. If this changes, there had to be a point between the two
    //just examined ones. In the while-loop this point is approached and given back, when
    //epsilon is bigger than the left inaccuracy.
    int foundPoints = 0;
    int sign = 0; //Algebraic sign which signals direction of vector to point against normal in point

    for(uint i = 0; i < sampleValues.size(); ++i) {
        int throughOrigin = testThroughOrigin(sampleValues[i], sign, foundPoints, samples, epsilon);
        if(throughOrigin < 0) {
            real a = sampleValues[i - 1]; //this point must exist, as sampleValues[0] can not reach this if condition
            real b = sampleValues[i];
            while(throughOrigin != 1) {
                throughOrigin = testThroughOrigin((a + b) / 2, sign, foundPoints, samples, epsilon);
                if(throughOrigin == -1) {
                    b = (a + b) / 2;
                } else if (throughOrigin == 0) {
                    a = (a + b) / 2;
                }
            }
        }
    }
    return foundPoints;
}

// Returns:  1 ==> point found and inserted in samples with foundPoints increased by 1
//           0 ==> no point found, direction same as in last test
//          -1 ==> no point found, but direction changed => Please check values between!
int SplineToothProfile::testThroughOrigin(real evaluationValue, int &lastSign, int &foundPoints, vector<vec2> &samples, real epsilon) const {
    vec2 p = evaluate(evaluationValue);
    vec2 n = normal(evaluationValue);
    real direction = p.x() * n.y() - p.y() * n.x();
    int currentSign = (direction < 0) ? -1 : 1;
    if((currentSign > 0 && direction < epsilon) ||
        (currentSign < 0 && (-1.0f * direction) < epsilon)) {
        samples[foundPoints] = p;
        foundPoints += 1;
        lastSign = 0;
        return 1;
    } else if (lastSign * currentSign == -1) { //pointing to different directions
        return -1;
    } else {
        lastSign = currentSign; //needed for the cases, when lastSign == 0
        return 0;
    }
}
