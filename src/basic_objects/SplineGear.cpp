#include "helpers.h"
#include "basic_objects/SplineGear.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

SplineGear::SplineGear(const Spline &toothOfGear) : m_toothProfile(new Spline(toothOfGear)),
    m_referenceRadius(0.0),
    m_numberOfTeeth(0),
    m_rotationDirection(0),
    m_minimumDistanceToCenter(-1.0),
    m_maximumDistanceToCenter(-1.0)
{
    std::cout << "SplineGear is created" << std::endl;
    update();
}

SplineGear::SplineGear(const SplineGear &other) : m_toothProfile(new Spline(*other.m_toothProfile)),
    m_referenceRadius(other.m_referenceRadius),
    m_numberOfTeeth(other.m_numberOfTeeth),
    m_rotationDirection(other.m_rotationDirection),
    m_minimumDistanceToCenter(other.m_minimumDistanceToCenter),
    m_maximumDistanceToCenter(other.m_maximumDistanceToCenter)
{
    std::cout << "SplineGear is created with copy constructor" << std::endl;
    update();
}

SplineGear::~SplineGear() {
    std::cout << "SplineGear is deleted" << std::endl;
    delete m_toothProfile;
}

void SplineGear::update() {
    m_degree = m_toothProfile->degree();

    if(isValid()) {
        if(m_rotationDirection == 0)
            m_rotationDirection = toothDescribedInClockDirection() ? 1 : -1;
        if(m_numberOfTeeth == 0) {
            // If the tooth profile would suit 9.x times in a circle, only use 9 teeth to avoid overlapping start and end points
            m_numberOfTeeth = maximumPossibleToothCount();
        }
        updateKnotsAndControlPoints();
        updateDistancesToCenter();
        // if no radius given or form of the gear has changed, set the reference radius to new value
        if(absolute(m_referenceRadius - defaultReferenceRadius()) > 0.0001) {
            m_referenceRadius = defaultReferenceRadius();
        }

    } else if(m_toothProfile->numberOfControlPoints() == 1) {
        if(m_rotationDirection == 0)
            m_rotationDirection = 1;
        if(m_numberOfTeeth <= 0)
            m_numberOfTeeth = 6; // No special number, but there has to be one
        updateControlPoints();
        m_referenceRadius = glm::length(m_toothProfile->controlPoint(0));

    } else if(m_toothProfile->numberOfControlPoints() > 1) {
        vec2 first = m_toothProfile->controlPoint(0);
        vec2 last = m_toothProfile->controlPoint(m_toothProfile->numberOfControlPoints() - 1);
        if(m_rotationDirection == 0)
            m_rotationDirection = (first.x * last.y - first.y * last.x > 0.0) ? 1 : -1;
        if(m_numberOfTeeth <= 0) {
            real angleOfControlPoints = angleBetween(first, last);
            m_numberOfTeeth = static_cast<uint>(floor((2.0 * M_PI) / angleOfControlPoints));
        }
        updateControlPoints();
        m_referenceRadius = glm::length(vec2((first + last) * 0.5));

    } else { // no control points specified
        setBackKnotsAndControlPoints();
    }
}

// We assume a coordinate system like that of screen coordinates. So the x-axis
// is pointing to the right, the y-axis points downward. Therefore "in clock direction"
// is a mathematical "counter clock direction"
bool SplineGear::toothDescribedInClockDirection() const {
    vec2 first = m_toothProfile->firstPoint();
    vec2 last = m_toothProfile->lastPoint();
    return (first.x * last.y - first.y * last.x > 0.0);
    //is 0 if points lie on one line through the origin.
}

real SplineGear::module() const {
    if(m_numberOfTeeth <= 0.0)
        return 0.0;
    return 2.0 * m_referenceRadius / m_numberOfTeeth;
}

void SplineGear::setModule(real module) {
    if(module <= 0.0)
        return;
    setReferenceRadius(module * m_numberOfTeeth / 2.0);
}

real SplineGear::defaultReferenceRadius() const {
    // Assume for simplicity a radius which lies in the middle of max and min points around the center
    return (maximumDistanceToCenter() + minimumDistanceToCenter()) / 2.0;
}

void SplineGear::setReferenceRadius(real radius) {
    if(radius <= 0.0)
        return;
    real scaling = radius / m_referenceRadius;
    m_toothProfile->scale(scaling);
    update();
    m_referenceRadius = defaultReferenceRadius();
}

real SplineGear::referenceRadius() const {
    return m_referenceRadius;
}

uint SplineGear::numberOfTeeth() const {
    return m_numberOfTeeth;
}

void SplineGear::setNumberOfTeeth(uint numberOfTeeth) {
    m_numberOfTeeth = numberOfTeeth;
    update();
}

real SplineGear::angularPitch() const { //Teilungswinkel
    return 2.0 * M_PI / m_numberOfTeeth;
}

real SplineGear::maximumDistanceToCenter() const {
    return m_maximumDistanceToCenter;
}

real SplineGear::minimumDistanceToCenter() const {
    return m_minimumDistanceToCenter;
}

real SplineGear::maximumDistanceOfControlPointToCenter() const {
    return m_toothProfile->maximumDistanceOfControlPointToOrigin();
}

real SplineGear::minimumDistanceOfControlPointToCenter() const {
    return m_toothProfile->minimumDistanceOfControlPointToOrigin();
}

uint SplineGear::maximumPossibleToothCount() const {
    //minimum angular pitch with m_toothProfile:
    vec2 start = m_toothProfile->firstPoint();
    vec2 stop  = m_toothProfile->lastPoint();
    // if angularPitch > PI (more than half of circle), concstruction will fail!
    real minAngularPitch = angleBetween(start, stop);

    return static_cast<uint>(floor((2.0 * M_PI) / minAngularPitch));
}

vec2 SplineGear::startPointForTooth() const {
    return m_toothProfile->firstPoint();
}

Spline* SplineGear::completeToothProfile() const {
    if(!isValid())
        return nullptr;

    //Create a spline of two following teeth
    uint numberOfControlPoints = m_toothProfile->numberOfControlPoints() + m_degree;
    uint numberOfKnots = numberOfControlPoints + m_degree - 1;
    vector<real> knots(numberOfKnots);
    vector<vec2> controlPoints(numberOfControlPoints);
    for(uint i = 0; i < numberOfKnots; ++i) {
        knots[i] = m_knots[i];
    }
    for(uint i = 0; i < numberOfControlPoints; ++i) {
        controlPoints[i] = m_controlPoints[i];
    }

    return new Spline(controlPoints, knots);
}

void SplineGear::updateDistancesToCenter() {
    if(m_numberOfTeeth == 1) {
        m_minimumDistanceToCenter = this->minimumDistanceToOrigin();
        m_maximumDistanceToCenter = this->maximumDistanceToOrigin();
    } else {
        //Create a spline of two following teeth
        uint knotsSize = 2 * m_toothProfile->numberOfControlPoints() + m_degree - 1;
        vector<real> knots(knotsSize);
        vector<vec2> controlPoints(2 * m_toothProfile->numberOfControlPoints());
        for(uint i = 0; i < knotsSize; ++i) {
            knots[i] = m_knots[i];
        }
        for(uint i = 0; i < controlPoints.size(); ++i) {
            controlPoints[i] = m_controlPoints[i];
        }
        Spline s(controlPoints, knots);
        m_minimumDistanceToCenter = s.minimumDistanceToOrigin();
        m_maximumDistanceToCenter = s.maximumDistanceToOrigin();
    }
}

/*Calculate the curve representing the whole gear by putting together many of copies
of the m_toothProfile. It doesn't matter, if the spline is torn to edges or not, but as
no control point is removed, if it is torn to the edges, the edge point is moved to the
first control point of each tooth.

Example 1 (9 control points, degree = 3, numberOfTeeth = 4, tornToEdges = true):

Given knots: 0---0---0---1---2---5---6---8---9---9---9
New knots:   0---0---0---1---2---5---6---8---9
            10--10--10--11--12--15--16--18--19
            20--20--20--21--22--25--26--28--29
            30--30--30--31--32--35--36--38--39
            40--40--40                        --41--42
The second line can not continue with the value "9" as otherwise the spline would
have four times the same knot with value "9"! Therefore another higher value is necessary
on this connection. In such a case there are two possible increasing solutions taken
into account. If a higher value further behind in the m_toothProfile is found, this
one is taken. If there isn't one, simply "1" is added to the last found value to continue
in the next line. This is the case in this example.
The first three knots of the last line are necessary to close the gear curve and the last
two knots for the spline of degree 3.

Example 2 (5 control points, degree = 3, numberOfTeeth = 4, tornToEdges = false):

Given knots: 1---3---4---5---8--10--11
New knots:   1---3---4---5---8
            10--13--14--15--18
            20--23--24--25--28
            30--33--34--35--38
            40--43--44        --45--48
The first three knots of the last line are necessary for the smooth connection of the curve
to form a circular curve. The last two knots are required for the degree of 3.

Example 3 (5 control points, degree = 4, numberOfTeeth = 4, tornToEdges = false):

Given knots: 1---3---4---5---8--10--11--13
New knots:   1---3---4---5---8
            10--13--14--15--18
            20--23--24--25--28
            30--33--34--35--38
            40--43--44--45    --48--50--53
The first four knots of the last line are necessary for the smooth connection of the curve
to form a circular curve. The last three knots are required for the degree of 4.
*/
void SplineGear::updateKnotsAndControlPoints() {
// UPDATE CONTROL POINTS:
    updateControlPoints();

// UPDATE KNOTS
    uint ppt = m_toothProfile->numberOfControlPoints(); // ppt = points per tooth
    // Store the toothProfile knots here for better access, as they are accessed very often
    const vector<real> givenKnots = m_toothProfile->knots();

    // Create a temporary vector, in which the spacing between two following knots is saved.
    // This simplifies the extension of the knots
    vector<real> knotInterspaces(ppt);
    for(uint i = 0; i < ppt; ++i) {
        // When degree is 1, there are not enough knots to define a continuation of the knot vector for the gear
        // Therefore another knot value must be added
        if(i >= givenKnots.size() - 1) {
            assert(m_degree == 1 && i == (ppt - 1));
            if(i == 0)
                knotInterspaces[i] = 1.0;
            else
                knotInterspaces[i] = knotInterspaces.at(i - 1);
        } else {
            knotInterspaces[i] = givenKnots[1 + i] - givenKnots[i];
        }
    }

    // control problematic multiple similar knots:
    //... at the beginning:
    uint k_1 = 0;
    uint l_1 = k_1 + 1;
    //... and at the end:
    uint l_2 = ppt; // = givenKnots.size() - 1 - m_degree
    uint k_2 = ppt - 1;


    if(givenKnots[k_1] == givenKnots[l_1] && givenKnots[k_2] == givenKnots[l_2]) {
        // problematic are only splines with above condition
        uint i = 0;
        bool foundSame = true;
        while(i < m_degree - 2 && foundSame) { //we already have two equal knots: at k_1 and l_1
            i += 1;
            if(givenKnots[l_1] != givenKnots[l_1 + i])
                foundSame = false;
        }
        uint similarStartKnots = i + 1;

        i = 0;
        foundSame = true;
        while(i < m_degree - 2 && foundSame) {
            i += 1;
            if(givenKnots[k_2] != givenKnots[k_2 - i])
                foundSame = false;
        }
        uint similarEndKnots = i;

        if(similarStartKnots + similarEndKnots > m_degree) {
            // knot values must be adapted for concatenation
            // choose a new value for step to next tooth
            real newLastValue = givenKnots[l_2] + 1.0; //<== This may be a bad choice, but at least it's a simple calculation
            // for a better choice how to carry on, test if a different knot value exists
            i = 1;
            foundSame = true;
            while(i < m_degree && foundSame) {
                if(givenKnots[l_2] != givenKnots[l_2 + i]) {
                    newLastValue = givenKnots[l_2 + i];
                    foundSame = false;
                }
            }
            knotInterspaces[ppt - 1] = newLastValue - givenKnots[ppt - 1];
        }
    }

    // Now finally fill new knot vector
    m_knots.resize(m_controlPoints.size() + m_degree - 1); //m_controlPoints are already updated!
    m_knots[0] = givenKnots[0];
    for(uint tooth = 0; tooth < m_numberOfTeeth; ++tooth) {
        for(uint j = 1; j <= ppt; ++j) {
            uint position = tooth * ppt + j;
            m_knots[position] = m_knots[position - 1] + knotInterspaces[j - 1];
        }
    }
    uint lastFilledPosition = m_numberOfTeeth * ppt;    // there's already one knot inserted of the last "line"!
    for(uint i = 1; i < 2 * m_degree - 1; ++i) {        // so only add (2 * degree - 2) knots
        m_knots[lastFilledPosition + i] = m_knots[lastFilledPosition + i - 1] + knotInterspaces[(i - 1) % ppt]; //as 2 * m_degree may be greater than ppt, which is the size of the knotInterspaces vector, the modulo is necessary here
    }
}

/*
Whereas the knots have to be put together in a suitable way, the control points
only need to be rotated, so for a gear with 4 teeth and 9 control points in the
m_toothProfile, we get:

Given points: a--b--c--d--e--f--g--h--i
New points:   a0-b0-c0-d0-e0-f0-g0-h0-i0
              a1-b1-c1-d1-e1-f1-g1-h1-i1
              a2-b2-c2-d2-e2-f2-g2-h2-i2
              a3-b3-c3-d3-e3-f3-g3-h3-i3

Remark: The corresponding points starting from line two are rotated around the origin.
*/
void SplineGear::updateControlPoints() {
    uint ppt = m_toothProfile->numberOfControlPoints(); // ppt = points per tooth

    m_controlPoints.resize(m_numberOfTeeth * ppt + m_degree); // the '+ m_degree' is necessary to close the curve

    for(uint tooth = 0; tooth < m_numberOfTeeth; ++tooth) {
        real rotationInRad = (real)tooth * (real)m_rotationDirection * angularPitch();
        for(uint j = 0; j < ppt; ++j) {
            m_controlPoints[j + tooth * ppt] = glm::rotate(m_toothProfile->controlPoint(j), rotationInRad); //rotate the controlPoint[j] by (2 * M_PI) / m_numberOfTeeth
        }
    }

    // add the points to close the curve
    for(uint i = 0; i < m_degree; ++i) {
        m_controlPoints[m_numberOfTeeth * ppt + i] = m_controlPoints[i];
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
    m_rotationDirection = 0; //set back the rotation direction as it may have changed
    update();
}

void SplineGear::addControlPoint(real x, real y) {
    addControlPoint(vec2(x, y));
}

void SplineGear::setControlPointsForTooth(const vector<vec2> &newControlPoints) {
    m_toothProfile->setControlPoints(newControlPoints);
    m_rotationDirection = 0; //set back the rotation direction as it may have changed
    update();
}

void SplineGear::removeControlPoint(uint index) {
    m_toothProfile->removeControlPoint(relatedIndexInFirstTooth(index));
    m_rotationDirection = 0;
    update();
}

void SplineGear::removeAllControlPoints() {
    m_toothProfile->removeAllControlPoints();
    setBackKnotsAndControlPoints();
    m_rotationDirection = 0;
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

void SplineGear::knotRefinement(real maxDist) {
    m_toothProfile->knotRefinement(maxDist);
    update();
}

void SplineGear::cutOffEdges() {
    m_toothProfile->cutOffEdges();
    update();
}

void SplineGear::refineEverywhere() {
    m_toothProfile->refineEverywhere();
    update();
}

void SplineGear::cutOffEdgesAndRefine() {
    m_toothProfile->cutOffEdges();
    m_toothProfile->refineEverywhere();
    update();
}

void SplineGear::refineAt(real knotValue, uint multiplicity) {
    real valueInFirstTooth = relatedKnotValueInFirstTooth(knotValue);
    m_toothProfile->refineAt(valueInFirstTooth, multiplicity);
    update();
}

void SplineGear::setClosedCurve(bool isClosed) {
    Q_UNUSED(isClosed);
}

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
    real rotationInRad = (real)m_rotationDirection * (real)toothIndex * angularPitch();
    return glm::rotate(positionInFirstTooth, rotationInRad);
}

void SplineGear::setControlPoints(const vector<vec2> &newControlPoints) {
    setControlPointsForTooth(newControlPoints);
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
    real rotationInRad = -((real)m_rotationDirection * (real)toothIndex * angularPitch()); //the minus sign is necessary, as the first tooth from a following tooth is searched
    return glm::rotate(position, rotationInRad);
}

real SplineGear::relatedKnotValueInFirstTooth(real gearValue) const {
    uint knotIndex = lowerNextKnot(gearValue);
    if(knotIndex < m_toothProfile->numberOfControlPoints())
        return gearValue; //value belongs to first tooth

    real knotIncreasementPerTooth = m_knots[m_toothProfile->numberOfControlPoints()] - m_knots[0];
    real gearValueWithoutOffset = gearValue - m_knots[0];

    int toothNumber;
    //store the quotient of 'gearValueWithoutOffset / knotIncreasementPerTooth' in toothNumber
    remquo(gearValueWithoutOffset, knotIncreasementPerTooth, &toothNumber);

    return gearValue - toothNumber * knotIncreasementPerTooth;
}
