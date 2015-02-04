#include "SplineGear.h"
#include <Eigen/Geometry>

#include "helpers.h"

SplineGear::SplineGear(SplineToothProfile *toothOfGear) : m_toothProfile(toothOfGear) {
    std::cout << "SplineGear is created" << std::endl;
    m_radius = 0.0f;
    m_numberOfTeeth = 0;
    update();
}

SplineGear::~SplineGear() {
    std::cout << "SplineGear is deleted" << std::endl;
}

void SplineGear::update() {
    m_degree = m_toothProfile->degree();
    if(isValid()) {
        if(m_radius <= 0) {
            // Assume for simplicity a radius which lies in the middle of max and min points around the center
            m_radius = (maximumDistanceToCenter() + minimumDistanceToCenter()) / 2.0f;
        }
        if(m_numberOfTeeth == 0) {
            // If the tooth profile would suit 9.x times in a circle, only use 9 teeth to avoid overlapping start and end points
            m_numberOfTeeth = maximumPossibleToothCount();
        }
        updateKnotsAndControlPoints();
    } else {
        setBackKnotsAndControlPoints();
    }
}

// We assume a coordinate system like that of screen coordinates. So the x-axis
// is pointing to the right, the y-axis points downward. Therefore "in clock direction"
// is a mathematical "counter clock direction"
bool SplineGear::toothDescribedInClockDirection() const {
    vec2 first = m_toothProfile->evaluate(m_toothProfile->lowerDomainLimit());
    vec2 last = m_toothProfile->evaluate(m_toothProfile->upperDomainLimit() - 0.01f);
    return (first.x() * last.y() - first.y() * last.x() > 0.0f);
    //is 0 if points lie on one line through the origin.
}

uint SplineGear::numberOfTeeth() const {
    return m_numberOfTeeth;
}

void SplineGear::setNumberOfTeeth(uint numberOfTeeth) {
    m_numberOfTeeth = numberOfTeeth;
    update();
}

real SplineGear::angularPitch() const { //Teilungswinkel
    return 2.0f * M_PI / m_numberOfTeeth;
}

real SplineGear::maximumDistanceToCenter() const {
    return m_toothProfile->maximumDistanceToOrigin();
}

real SplineGear::minimumDistanceToCenter() const {
    return m_toothProfile->minimumDistanceToOrigin();
}

real SplineGear::maximumDistanceOfControlPointToCenter() const {
    return m_toothProfile->maximumDistanceOfControlPointToOrigin();
}

real SplineGear::minimumDistanceOfControlPointToCenter() const {
    return m_toothProfile->minimumDistanceOfControlPointToOrigin();
}

uint SplineGear::maximumPossibleToothCount() const {
    //minimum angular pitch with m_toothProfile:
    vec2 start = m_toothProfile->start();
    vec2 stop  = m_toothProfile->stop();
    // if angularPitch > PI (more than half of circle), concstruction will fail!
    real minAngularPitch = acos((start.normalized().dot(stop.normalized())));

    return static_cast<uint>(floor((2.0f * M_PI) / minAngularPitch));
}

vec2 SplineGear::startPointForTooth() const {
    return m_toothProfile->start();
}

/*Returns a new BSplineCurve representing the whole gear by putting together
many of copies of the tooth profile bspline. It doesn't matter, if the spline is torn
to edges or not, but as no control point is removed, if it is torn to the edges,
the edge point is moved to the first control point of each tooth.

Example 1 (9 control points, degree = 3, numberOfTeeth = 4, tornToEdges = true):

Given knots: 0---0---0---0---1---2---5---6---8---9---9---9---9
New knots:   0---0---0---0---1---2---5---6---8---9
                10--10--10--11--12--15--16--18--19
                20--20--20--21--22--25--26--28--29
                30--30--30--31--32--35--36--38--39
                40                                --40--40--40
The last 4 knots are needed to close the gear curve. As it is a connection with 3 times
the same knot, only one connecting control point is needed and not 'degree' control points.

Given points: a--b--c--d--e--f--g--h--i
New points:   a0-b0-c0-d0-e0-f0-g0-h0-i0
              a1-b1-c1-d1-e1-f1-g1-h1-i1
              a2-b2-c2-d2-e2-f2-g2-h2-i2
              a3-b3-c3-d3-e3-f3-g3-h3-i3

Example 2 (5 control points, degree = 3, numberOfTeeth = 4, tornToEdges = false):

Given knots: 0---1---3---4---5---8--10--11--13
New knots:   0---1---3---4---5---8
                10--13--14--15--18
                20--23--24--25--28
                30--33--34--35--38
                40--43--44         --45--48--50
The three knots of the last line are necessary for the smooth connection of the curve
to form a circyc curve.

Remark: The numbers behind the literals mean, that the corresponding points
are rotated around the origin. As point a equals point i when rotated with
one anuglar pitch, point i is dropped
*/
void SplineGear::updateKnotsAndControlPoints() {
    int rotationDirection = (toothDescribedInClockDirection()) ? 1 : -1;

    // Update control points:
    uint pPT = m_toothProfile->numberOfControlPoints(); // pPT = points per tooth
    m_controlPoints.resize(m_numberOfTeeth * pPT + m_degree); // the '+ m_degree' is necessary to close the curve
    for(uint tooth = 0; tooth < m_numberOfTeeth; ++tooth) {
        real rotationInRad = tooth * rotationDirection * angularPitch();
        for(uint j = 0; j < pPT; ++j) {
            m_controlPoints[j + tooth * pPT] = Eigen::Rotation2D<real>(rotationInRad) * m_toothProfile->controlPoint(j); //rotate the controlPoint[j] by (2 * M_PI) / m_numberOfTeeth
        }
    }

    // add the points to close the curve
    for(uint i = 0; i < m_degree; ++i) {
        m_controlPoints[m_numberOfTeeth * pPT + i] = m_toothProfile->controlPoint(i);
    }

    // Update knots:
    const vector<real> givenKnots = m_toothProfile->knots();
    // Create a temporary knotOffset vector to simplify extension of knots
    vector<real> knotOffsets(pPT);
    for(uint i = 0; i < pPT; ++i) {
        knotOffsets[i] = givenKnots[2 + i] - givenKnots[1 + i];
    }

    // control problematic multiple similar knots:
    uint k_1 = 1;
    uint k_2 = givenKnots.size() - 1 - m_degree;
    uint l_1 = k_1 + 1;
    uint l_2 = k_2 + 1;
    if(givenKnots[k_1] == givenKnots[l_1] && givenKnots[k_2] == givenKnots[l_2]) {
        // problematic are only splines with above condition
        uint i = 0;
        bool foundSame = true;
        while(i < m_degree - 2 && foundSame) {
            i += 1;
            if(givenKnots[l_1] != givenKnots[l_1 + i])
                foundSame = false;
        }
        uint similarStartKnots = i + 1;

        i = 0;
        foundSame = true;
        while(i < m_degree - 1 && foundSame) {
            i += 1;
            if(givenKnots[k_2] != givenKnots[k_2 - i])
                foundSame = false;
        }
        uint similarEndKnots = i;

        if(similarStartKnots + similarEndKnots > m_degree) {
            // knot values must be adapted for concatenation
            // choose a new value for continuation
            real newLastValue = givenKnots[l_2] + 1.0f; //<== This may be a bad choice, but at least it's a simple calculation
            // for a better choice how to carry on, test if a different knot value exists
            i = 1;
            foundSame = true;
            while(i < m_degree && foundSame) {
                if(givenKnots[l_2] != givenKnots[l_2 + i]) {
                    newLastValue = givenKnots[l_2 + i];
                    foundSame = false;
                }
            }
        }
    }

    // Now finally fill new knot vector
    m_knots.resize(m_controlPoints.size() + m_degree + 1);
    m_knots[0] = givenKnots[0];
    for(uint tooth = 0; tooth < m_numberOfTeeth; ++tooth) {
        for(uint j = 1; j <= pPT; ++j) {
            uint position = tooth * pPT + j;
            m_knots[position] = m_knots[position - 1] + knotOffsets[j - 1];
        }
    }
    uint lastFilledPosition = m_numberOfTeeth * pPT;
    for(uint i = 1; i <= 2 * m_degree; ++i) {
        m_knots[lastFilledPosition + i] = m_knots[lastFilledPosition + i - 1] + knotOffsets[i - 1];
    }
}

void SplineGear::setBackKnotsAndControlPoints() {
    m_knots.resize(0);
    m_controlPoints.resize(0);
}

void SplineGear::setDegree(uint degree) {
    m_toothProfile->setDegree(degree);
    update();
}

void SplineGear::addControlPoint(vec2 point) {
    m_toothProfile->addControlPoint(point);
    update();
}

void SplineGear::addControlPoint(real x, real y) {
    addControlPoint(vec2(x, y));
}

void SplineGear::removeControlPoint(uint index) {
    m_toothProfile->removeControlPoint(relatedIndexInFirstTooth(index));
    update();
}

void SplineGear::moveControlPoint(uint index, vec2 newPosition) {
    vec2 newPositionInTooth = relatedPositionInFirstTooth(toothIndex(index), newPosition);
    m_toothProfile->moveControlPoint(relatedIndexInFirstTooth(index), newPositionInTooth);
    update();
}

void SplineGear::moveControlPoint(uint index, QPointF newPosition) {
    moveControlPoint(index, vec2(newPosition.x(), newPosition.y()));
}

void SplineGear::setClosedCurve(bool isClosed) {}

bool SplineGear::isClosed() const {
    return true;
}

void SplineGear::updateClosedCurve() {}

void SplineGear::setTornToEdges(bool tearToEdges) {
    if(isTornToEdges() == tearToEdges)
        return;
    m_toothProfile->setTornToEdges(tearToEdges);
    update();
}

bool SplineGear::isTornToEdges() const {
    return m_toothProfile->isTornToEdges();
}

bool SplineGear::checkTornToEdges() const {
        return m_toothProfile->checkTornToEdges();
}

void SplineGear::updateTornToEdges() {
    m_toothProfile->updateTornToEdges();
}

bool SplineGear::isValid() const {
    return m_toothProfile->isValid();
}

vec2 SplineGear::relatedPositionInTooth(uint toothIndex, vec2 positionInFirstTooth) const {
    if(toothIndex == 0)
        return positionInFirstTooth;
    int rotationDirection = (toothDescribedInClockDirection()) ? 1 : -1;
    real rotationInRad = rotationDirection * toothIndex * angularPitch();
    return Eigen::Rotation2D<real>(rotationInRad) * positionInFirstTooth;
}

uint SplineGear::relatedIndexInFirstTooth(uint controlPointIndex) const {
    return controlPointIndex % m_toothProfile->numberOfControlPoints();
}

uint SplineGear::toothIndex(uint controlPointIndex) const {
    return static_cast<uint>(floor(controlPointIndex / m_toothProfile->numberOfControlPoints()));
}

vec2 SplineGear::relatedPositionInFirstTooth(uint toothIndex, vec2 position) const {
    if(toothIndex == 0)
        return position;
    int rotationDirection = (toothDescribedInClockDirection()) ? 1 : -1;
    real rotationInRad = -(rotationDirection * toothIndex * angularPitch()); //the minus sign is necessary, as the first tooth from a following tooth is searched
    return Eigen::Rotation2D<real>(rotationInRad) * position;
}
