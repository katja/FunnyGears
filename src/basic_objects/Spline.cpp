#include "helpers.h"
#include "basic_objects/Spline.h"
#include <cmath>
#include <glm/glm.hpp>

Spline::Spline(uint degree) : m_degree(degree), m_isClosed(false), m_tornToEdges(false) {
    if(m_degree <= 0)
        throw;
    for(uint i = 1; i < m_degree; ++i)
        m_knots.push_back(i);
}

Spline::Spline(const Spline &other) {
    m_knots = vector<real>(other.m_knots);
    m_controlPoints = vector<vec2>(other.m_controlPoints);
    m_degree = other.m_degree;
    m_isClosed = other.m_isClosed;
    m_tornToEdges = other.m_tornToEdges;
}

Spline::Spline(vector<vec2> controlPoints, vector<real> knots) {
    // m_degree is set on relation between number of knots and number of control points.
    // but if following is not given, m_degree would have to be null or negative! => not possible
    if(knots.size() < controlPoints.size())
        throw;
    m_knots = knots;
    m_controlPoints = controlPoints;
    m_degree = m_knots.size() - m_controlPoints.size() + 1;
    updateTornToEdges();
    updateClosedCurve();
}

Spline::Spline(vector<vec2> interpolationPoints) {
    //TODO!!!
}

Spline::~Spline() {
}

Spline& Spline::operator=(const Spline &other) {
    if(this == &other)
        return *this;
    m_knots.clear();
    m_knots.resize(other.m_knots.size());
    for(uint i = 0; i < m_knots.size(); ++i) {
        m_knots[i] = other.m_knots.at(i);
    }
    m_controlPoints.clear();
    m_controlPoints.resize(other.m_controlPoints.size());
    for(uint i = 0; i < m_controlPoints.size(); ++i) {
        m_controlPoints[i] = other.m_controlPoints.at(i);
    }
    m_degree = other.m_degree;
    m_isClosed = other.m_isClosed;
    m_tornToEdges = other.m_tornToEdges;
    return *this;
}

vec2 Spline::evaluate(real value) const {
    uint n = lowerNextKnot(value); // value is element of range [knot[n], knot[n+1])
    vector<vec2> controlPointsCopy(m_degree + 1);
    //TODO: find bug and delete following lines!
    if((n - m_degree + 1) < 0 || (n - m_degree + 1) >= m_controlPoints.size() || n + 1 >= m_controlPoints.size()) {
        std::cerr << "ATTENTION! PROGRAM WILL QUIT UNEXPECTEDLY IN A MOMENT!!!" << std::endl;
        if((n - m_degree + 1) < 0)
            std::cerr << "1. Condition (< 0) was not met" << std::endl;
        if((n - m_degree + 1) >= m_controlPoints.size())
            std::cerr << "2. Condition (>= size) was not met" << std::endl;
        if(n + 1 >= m_controlPoints.size())
            std::cerr << "3. Condition (>= size) was not met" << std::endl;
        std::cerr << "Spline has following state:" << std::endl;
        std::cerr << (*this) << std::endl;
        std::cerr << "IF PROGRAM DOES NOT QUIT NOW, BUG WAS FOUND!!!" << std::endl;
    }
    for(uint i = 0; i <= m_degree; ++i) {
        controlPointsCopy[i] = m_controlPoints.at((n + 1 + i) - m_degree); //Attention: do NOT change the order in braces, as uint is used!
    }
    deBoor(controlPointsCopy, value, n);
    return controlPointsCopy.at(m_degree);
}

vec2 Spline::firstPoint() const {
    return evaluate(lowerDomainLimit());
}

vec2 Spline::lastPoint(real epsilon) const {
    return evaluate(upperDomainLimit(epsilon));
}

vec2 Spline::derivative(real value) const {
    if(!isValid() || value < lowerDomainLimit() || value >= upperDomainLimit(0.0))
        return vec2(0, 0);
    //TODO: is it necessary here to test these requirements???

    uint n = lowerNextKnot(value); // value is element of [knot[n], knot[n+1])
    vector<vec2> controlPointsCopy(m_degree + 1);
    for(uint i = 0; i < m_degree + 1; ++i) {
        controlPointsCopy[i] = m_controlPoints.at((n + 1 + i) - m_degree); //Attention: do NOT change the order in braces, as uint is used!
    }

    //Stop de Boor algorithm one step earlier than when evaluating a curve point
    deBoor(controlPointsCopy, value, n, m_degree, 1);
    return glm::normalize(vec2(controlPointsCopy.at(m_degree) - controlPointsCopy.at(m_degree - 1)));
}

vec2 Spline::normal(real value) const {
    vec2 dv = derivative(value);
    if(dv == vec2(0,0))
        return dv;
    return vec2(-dv.y, dv.x);
}

void Spline::curve(vector<QPointF> &curve) const {
    if(!isValid())
        return;
    real uStart = lowerDomainLimit();
    real uStop = upperDomainLimit();
    uint samples = curve.size();

    real step = (uStop - uStart) / (samples - 1);
    for(uint i = 0; i < samples; ++i) {
        real u = step * (real)i + uStart;
        if(i == samples - 1)
            u = uStop; // prevent by all means a overflow of uStop
        vec2 point = evaluate(u);
        curve[i] = QPointF(point.x, point.y);
        // Following is not needed any more, when instead of the former used simple
        // addition is the now used more complex calculation done in every loop.
        // The former solution was simpler, but in a bad case could result in an overflow
        // of uStop, which must not happen!
        // // update u as floating points are not very accurate
        // // but nevertheless the whole spline should be visible:
        // if((i & 0xF) == 0) {
        //     step = (uStop - u) / (samples - i);
        // }
    }
}

uint Spline::degree() const {
    return m_degree;
}

void Spline::setDegree(uint degree) {
    if(m_degree == degree || degree == 0)
        return;

    bool wasValid = isValid();
    if(m_degree < degree) {
        if(m_tornToEdges && isValid())
            makeDifferentLastKnots();
        for(uint i = m_degree; i < degree; ++i) {
            m_knots.push_back((m_knots.at(m_knots.size() - 1) + 1));
            m_degree += 1;
        }
    } else { // m_degree > degree
        for(uint i = m_degree; i > degree; --i) {
            m_knots.pop_back();
            m_degree -= 1;
        }
    }

    //State changed in a valid state. As the knot vector may be completely bumfuzzled,
    //an adjustment (check an adaption) of all knots has to be done.
    if(!wasValid && isValid()) {
        adjustKnots();
    }
    //Adapt knots only if spline is in a valid state. If it is not, there need to be
    //a change, which sets the spline in a valid state. This change then has to check the
    //correctness of the knots!
    else if(m_tornToEdges && isValid()) {
        equalizeFirstKnots();
        equalizeLastKnots();
    }
}

uint Spline::numberOfControlPoints() const {
    return m_controlPoints.size();
}

const vector<vec2>& Spline::controlPoints() const {
    return m_controlPoints;
}

vec2 Spline::controlPoint(uint i) const {
    return m_controlPoints[i];
}

void Spline::addControlPoint(vec2 point) {
    if(m_tornToEdges && isValid()) {
        makeDifferentLastKnots();
        if(m_knots.empty())
            m_knots.push_back(0.0);
        else
            m_knots.push_back((m_knots.back() + 1));
        m_controlPoints.push_back(point);
        equalizeLastKnots();
    } else {
        m_controlPoints.push_back(point);
        if(m_knots.empty())
            m_knots.push_back(0.0);
        else
            m_knots.push_back((m_knots.back() + 1));

        // If it is valid now, adjust (check and adapt) all knots, as the may be bumfuzzled.
        if(isValid())
            adjustKnots();
    }
}

void Spline::addControlPoint(real x, real y) {
    addControlPoint(vec2(x, y));
}

void Spline::setControlPoints(const vector<vec2> &newControlPoints) {
    m_controlPoints.clear();
    for(uint i = 0; i < newControlPoints.size(); ++i) {
        m_controlPoints.push_back(newControlPoints[i]);
    }
    int knotDifference = static_cast<int>(m_degree + m_controlPoints.size() - 1) - m_knots.size(); // (new knot size) - (old knot size)
    if(knotDifference < 0) {
        //old knot vector was larger => make shorter
        m_knots.erase(m_knots.end() + knotDifference, m_knots.end());
    } else if(knotDifference > 0) {
        //old knot vector was smaller => enlarge it
        for(int i = 0; i < knotDifference; ++i) {
            m_knots.push_back(m_knots.back() + 1);
        }
    }
    if(knotDifference != 0 && isValid())
        adjustKnots();
}

void Spline::removeControlPoint(uint index) {
    if(index >= m_controlPoints.size())
        return;
    m_controlPoints.erase(m_controlPoints.begin() + index);
    m_knots.pop_back();

    if(m_tornToEdges && isValid())
        equalizeLastKnots();

    updateClosedCurve();
}

void Spline::removeAllControlPoints() {
    m_controlPoints.clear();
    uint leftKnots = m_degree - 1;
    m_knots.erase(m_knots.begin() + leftKnots, m_knots.end());

    if(m_tornToEdges && isValid())
        equalizeLastKnots();

    updateClosedCurve();
}

void Spline::moveControlPoint(uint index, vec2 newPosition) {
    if(index >= m_controlPoints.size())
        return;
    m_controlPoints[index] = newPosition;
}

void Spline::moveControlPoint(uint index, QPointF newPosition) {
    moveControlPoint(index, vec2(newPosition.x(), newPosition.y()));
}

uint Spline::numberOfKnots() const {
    return m_knots.size();
}

const vector<real>& Spline::knots() const {
    return m_knots;
}

uint Spline::multiplicityOfKnotValue(real knotValue) const {
    uint count = 0;
    for(uint i = 0; i < m_knots.size(); ++i) {
        if(m_knots[i] == knotValue)
            ++count;
    }
    return count;
}

uint Spline::multiplicity(uint knotIndex) const {
    uint count = 1;
    //do NOT use uint here, as otherwise for loop will never finish!!!
    for(int i = knotIndex - 1; i >= 0 && m_knots[i] == m_knots[knotIndex]; --i)
        ++count;
    for(uint i = knotIndex + 1; i < m_knots.size() && m_knots[knotIndex] == m_knots[i]; ++i)
        ++count;
    return count;
}

void Spline::knotRefinement(real maxDist) {
    if(!isValid())
        return;
    //test each segment from lowest possible u value unto u = m_controlPoints.size() (must not be reached)
    uint i = lowerDomainKnotIndex();
    while(i < upperDomainKnotIndex()) {
        real dist = m_knots.at(i + 1) - m_knots.at(i);
        if(dist > maxDist) {
            real newValue = m_knots.at(i) + 0.5 * dist;
            refineAt(newValue);
        } else {
            ++i;
        }
    }
}

void Spline::cutOffEdges() {
    uint numberOfKnots = m_knots.size();
    int m = m_degree - multiplicity(lowerDomainKnotIndex());
    if(m < 0) {
        std::cerr << "ERROR! In Spline::cutOffEdges() a negative multiplicity was recognized!" << std::endl;
        return;
    } else if(m == 0) {
        return;
    }
    refineAt(lowerDomainLimit(), (uint)m);
    if(numberOfKnots != m_knots.size()) {
        m_knots.erase(m_knots.begin(), m_knots.begin() + (m_degree - 1));
        m_controlPoints.erase(m_controlPoints.begin(), m_controlPoints.begin() + (m_degree - 1));
    }
    numberOfKnots = m_knots.size();
    refineAt(upperDomainLimit(0.000001), m_degree);
    if(numberOfKnots != m_knots.size()) {
        m_knots.erase(m_knots.end() - m_degree, m_knots.end());
        m_controlPoints.erase(m_controlPoints.end() - m_degree, m_controlPoints.end());
    }
    checkTornToEdges();
}

void Spline::refineEverywhere() {
    if(!isValid())
        return;

    vector<real> knotsToInsert;
    for(uint i = lowerDomainKnotIndex(); i < upperDomainKnotIndex(); ++i) {
        if(m_knots[i] != m_knots[i + 1])
            knotsToInsert.push_back(0.5 * (m_knots[i] + m_knots[i + 1]));
    }
    for(uint i = 0; i < knotsToInsert.size(); ++i) {
        refineAt(knotsToInsert[i]);
    }
}

void Spline::refineAt(real knotValue, uint multiplicity) {
    //A multiplicity of 0 would mean no knot insertion and
    //an insertion with multiplicity > m_degree would lead to a hole in the spline curve!
    if(multiplicity == 0 || multiplicityOfKnotValue(knotValue) + multiplicity > m_degree)
        return;
    //Use de Boor scheme to get the new point(s)
    uint n = lowerNextKnot(knotValue); // knotValue is element of [knot[n], knot[n+1])
    vector<vec2> controlPointsCopy(m_degree + 1);
    for(uint i = 0; i <= m_degree; ++i) {
        controlPointsCopy[i] = m_controlPoints.at(n - m_degree + 1 + i);
    }

    vector<vec2> newPoints(m_degree + multiplicity - 1);

    //When the new knotVaue is inserted r-times (multiplicity = r) the new points are equal
    //to the points of the de boor scheme in the r-th column and the outer way to this column
    for(uint i = 1; i <= multiplicity; ++i) {

        //do only one step of the de boor algorithm each iteration
        //and only multiplicity times
        //(so for multiplicity == 1 only one step is done)
        deBoor(controlPointsCopy, knotValue, n, m_degree - i + 1, m_degree - i);

        if(i == multiplicity) {
            //copy all up to now calculated points to the new points
            //except first one, which didn't change (exists already in spline)
            for(uint j = 1; j <= m_degree; ++j) {
                newPoints[j - 1] = controlPointsCopy.at(j);
            }

        } else {
            //copy only last point, as it will be overwritten in next deBoor step
            newPoints[newPoints.size() - i] = controlPointsCopy.back();
        }
    }

    //copy now new points into control points vector:
    m_controlPoints.resize(m_controlPoints.size() + multiplicity);
    int n_int = (int) n;
    int i = m_controlPoints.size() - 1 - multiplicity;
    for(; i > n_int; --i) {
        m_controlPoints[i + multiplicity] = m_controlPoints.at(i);
    }
    //do NOT replace 'int j' by 'uint j'!!!
    for(int j = newPoints.size() - 1; j >= 0; --j, --i) {
        m_controlPoints[i + multiplicity] = newPoints[j];
    }
    for(uint k = 1; k <= multiplicity; ++k) {
        m_knots.insert(m_knots.begin() + n + k, knotValue);
    }
}

void Spline::setClosedCurve(bool isClosed) {
    //TODO!!!
}

bool Spline::isClosed() const {
    return m_isClosed;
}

void Spline::updateClosedCurve() {
    //TODO
}


void Spline::setTornToEdges(bool tearToEdges) {
    if(m_tornToEdges != tearToEdges) {
        m_tornToEdges = tearToEdges;
        if(isValid()) {
            if(m_tornToEdges) {
                equalizeFirstKnots();
                equalizeLastKnots();
            } else { // not torn to edges
                makeDifferentFirstKnots();
                makeDifferentLastKnots();
            }
        }
    }
}

bool Spline::isTornToEdges() const {
    return m_tornToEdges;
}

bool Spline::checkTornToEdges() const {
    if(!isValid())
        return false;
    real start = m_knots.at(0);
    real stop = m_knots.at(m_knots.size() - 1);
    for(uint i = 1; i < m_degree; ++i) {
        if(m_knots.at(i) != start
            || m_knots.at(m_knots.size() - 1 - i) != stop)
            return false;
    }
    return true;
}

void Spline::updateTornToEdges() {
    m_tornToEdges = checkTornToEdges();
}

bool Spline::isValidMessages(std::ostream &os) const {
    bool isValid = true;
    if(m_degree <= 0) {
        os << "Degree is too small, has a value of " << m_degree << ", but must be at least 1" << std::endl;
    }
    if(m_controlPoints.size() != m_knots.size() - m_degree + 1) {
        os << "Number of control points does not suite to number of knots and degree\n";
        os << "number of control points should be " << (m_knots.size() - m_degree + 1);
        os << "but is " << m_controlPoints.size() << std::endl;
        isValid = false;
    }
    uint occurences = 1;
    real lastValue = m_knots.at(0);
    for(uint i = 1; i < m_knots.size(); ++i) {
        if(lastValue == m_knots.at(i)) {
            ++occurences;
        } else if (lastValue > m_knots.at(i)) {
            os << "Knots are not sorted in ascending order at knot" << i-1 << " to " << i << std::endl;
            isValid = false;
        } else {
            lastValue = m_knots.at(i);
            occurences = 1;
        }
        if(occurences > m_degree) {
            os << "Multiplicity of knot " << i << " is too high (" << occurences << ")." << std::endl;
        }
    }
    return isValid;
}

bool Spline::isValid() const {
    if(m_degree <= 0)
        return false;
    if(m_controlPoints.size() != m_knots.size() - m_degree + 1)
        return false;
    return m_controlPoints.size() > m_degree; //first visible for degree + 1 control points
}

uint Spline::lowerDomainKnotIndex() const {
    return m_degree - 1;
}

uint Spline::upperDomainKnotIndex() const {
    return m_controlPoints.size() - 1;
}

real Spline::lowerDomainLimit() const {
    if(!isValid())
        return -1.0; //No curve available at the moment!
    return m_knots[m_degree - 1];
}

real Spline::upperDomainLimit(real epsilon) const {
    if(!isValid())
        return -1.0; //No curve available at the moment!
    return m_knots[m_controlPoints.size() - 1] - epsilon; //is equal to: m_knots[m_knots.size() - 1 - m_degree] - epsilon
}

uint Spline::lowerNextKnot(real value) const {
    for(uint i = 1; i < m_knots.size(); ++i) {
        if(m_knots[i] > value)
            return i - 1;
    }
    return m_knots.size() - 1;
}

real Spline::minimumDistanceOfControlPointToOrigin() const {
    if(m_controlPoints.size() <= 0)
        return 0.0;

    real minDistance = glm::length(m_controlPoints[0]);

    for(uint i = 1; i < m_controlPoints.size(); ++i) {
        real dist = glm::length(m_controlPoints[i]);
        if(dist < minDistance)
            minDistance = dist;
    }
    return minDistance;
}

real Spline::maximumDistanceOfControlPointToOrigin() const {
    if(m_controlPoints.size() <= 0)
        return 0.0;

    real maxDistance = glm::length(m_controlPoints[0]);

    for(uint i = 1; i < m_controlPoints.size(); ++i) {
        real dist = glm::length(m_controlPoints[i]);
        if(dist > maxDistance)
            maxDistance = dist;
    }
    return maxDistance;
}

real Spline::minimumDistanceToOrigin() const {
    vector<vec2> possiblePoints(2 * m_controlPoints.size());
    int numOfPossiblePoints = pointsWithNormalsThroughOrigin(possiblePoints);
    real minDistance = glm::length(possiblePoints[0]);
    for(int i = 1; i < numOfPossiblePoints; ++i) {
        real dist = glm::length(possiblePoints[i]);
        if(dist < minDistance)
            minDistance = dist;
    }
    return minDistance;
}

real Spline::maximumDistanceToOrigin() const {
    vector<vec2> possiblePoints(2 * m_controlPoints.size());
    int numOfPossiblePoints = pointsWithNormalsThroughOrigin(possiblePoints);
    real maxDistance = glm::length(possiblePoints[0]);
    for(int i = 1; i < numOfPossiblePoints; ++i) {
        real dist = glm::length(possiblePoints[i]);
        if(dist > maxDistance)
            maxDistance = dist;
    }
    return maxDistance;
}

void Spline::getIntersectionPointsWithRay(const Ray &ray, vector<vec2> &intersectionPoints) const {
    if(!isValid())
        return; //=> number of control points is not at least 2

    //first test the control point polygon.
    //If the ray has no intersection with it, there can't be an intersection with the spline curve, too.
    for(int i = 0; (uint)i < m_controlPoints.size() - 1; ++i) {
        vec2 startLine, endLine, intersectionPoint;
        startLine = m_controlPoints[i];
        endLine = m_controlPoints[i + 1];

        if(ray.intersect(startLine, endLine, intersectionPoint)) {

            //An intersection occured with a line of the control point polygon
            //When degree = 1, the spline curve is equal to the control point polygon
            //So return the found point.
            if(m_degree == 1) {
                intersectionPoints.push_back(intersectionPoint);
            }
            //For higher degrees create a new spline with the points and knots which influence
            //the line, where the intersection occured. Refine knots of new spline and
            //examine the resulting spline recursively.
            //In a spline with degree = d the control point i and (i+1) need the knots:
            //      i ... i + d
            //But as (d-1) control points before and after the examined line between two
            //control points influence the curve, we have to examine these points, too.
            //So, in a whole, we have to examine for the line between control points
            //i and (i+1) the knots:
            //      i - (d - 1) ... i + d + (d - 1) = i + 2 * d - 1
            else {
                //TODO: bessere abschätzung möglich? z.b. abstand an der stelle von kontrollpolygon???
                if(upperDomainLimit(0.0) - lowerDomainLimit() > 0.002) {

                    int startKnotIndex = i - (m_degree - 1);
                    uint stopKnotIndex = i + 2 * m_degree - 1;
                    int startCPIndex = i - (m_degree - 1);
                    uint lengthCP = 2 * m_degree; //on each side add (d-1) control points and we have the two of the line
                    // uint lengthKnots = 3 * m_degree - 1; <== We don't need this, but it helps understanding the new spline

                    if(startKnotIndex < 0) {
                        lengthCP += startCPIndex;
                        startKnotIndex = 0;
                        startCPIndex = 0; //when knot index is negative, control point index will be negative, too
                    }

                    if(stopKnotIndex >= m_knots.size()) {
                        lengthCP -= (stopKnotIndex - (m_knots.size() - 1));
                        stopKnotIndex = m_knots.size() - 1;
                    }
                    vector<vec2> controlPoints = copyVectorPart(m_controlPoints, startCPIndex, lengthCP);
                    vector<real> knots = copyVectorPart(m_knots, startKnotIndex, stopKnotIndex - startKnotIndex + 1);

                    //This new spline has (if it does not lie anywhere at the borders)
                    //(3 * degree - 1) knots and (2 * degree) control points
                    //So the validness is given:
                    //   degree =!= #knots + #ctrl-points + 1 = 3degree - 1 + 2degree + 1 = 1degree = degree
                    Spline splinePart(controlPoints, knots);
                    splinePart.cutOffEdges();
                    splinePart.refineEverywhere();
                    splinePart.getIntersectionPointsWithRay(ray, intersectionPoints);
                } else {
                    intersectionPoints.push_back(intersectionPoint);
                }
            }
        }
    }
}

void Spline::scale(real scaling, vec2 origin) {
    for(uint i = 0; i < m_controlPoints.size(); ++i) {
        m_controlPoints[i] = scaling * (m_controlPoints[i] - origin);
        m_controlPoints[i] = m_controlPoints[i] + origin;
    }
}

void Spline::deBoor(vector<vec2> &controlPoints, real value, real n) const {
    deBoor(controlPoints, value, n, m_degree, 0);
}

void Spline::deBoor(vector<vec2> &controlPoints, real value, real n, uint degree, uint stop) const {
    if(stop == degree)
        return;
    assert(controlPoints.size() >= degree + 1 && degree >= 0);
    uint lastIndex = controlPoints.size() - 1;

    for(int i = n, index = lastIndex; i > n - degree; --i, --index) {
        real alpha = (value - m_knots.at(i)) / (m_knots.at(i + degree) - m_knots.at(i));
        controlPoints[index] =
            (1.0 - alpha) * controlPoints.at(index - 1)
                + alpha * controlPoints.at(index);
    }
    degree -= 1;
    if(stop != degree)
        deBoor(controlPoints, value, n, degree, stop);
}

void Spline::adjustKnots() {
    if(!isValid())
        return;
    real lastKnotValue = m_knots[0];
    uint occurences = 1;

    for(uint i = 1; i < m_knots.size(); ++i) {

        //when torn to edges the first n = m_degree knots must have the same value
        if(m_tornToEdges && i < m_degree) {
            m_knots[i] = lastKnotValue;
            ++occurences;
        } else
        //when torn to edges the last knots must have the same value, starting with the
        //knot at upperDomainKnotIndex() = m_controlPoints.size() - 1
        //If this knot has the same value as the one before, give it a new bigger value.
        if(m_tornToEdges && i == upperDomainKnotIndex()) {
            if(m_knots.at(i) == lastKnotValue)
                m_knots[i] = lastKnotValue + 1;
            occurences = 1;
        } else

        //when torn to edges the last n = m_degree knots need the same value
        if(m_tornToEdges && i > m_knots.size() - 1 - m_degree) {
            assert(occurences < m_degree); //otherwise, number of control points to knots and to degree is not correct!
            m_knots[i] = lastKnotValue;
            ++occurences;
        } else

        //assure that there are not more than n = m_degree same knot values
        if(absolute(m_knots.at(i) - lastKnotValue) < 0.00000001) {
            if(m_knots.at(i) - lastKnotValue != 0)
                std::cerr << "ATTENTION! Spline::adjustKnots() had found knots with different values but handled as same value" << std::endl; // Previously only real equality was tested, so if this is found the current state behaves different from previous states!!!
            ++occurences;
            if(occurences > m_degree) {
                m_knots[i] = lastKnotValue + 1;
                occurences = 1;
            }

        //m_knots.at(i) != lastKnotValue
        } else {
            //assure that knots have an increasing order
            if(m_knots.at(i) < lastKnotValue)
                m_knots[i] = lastKnotValue + 1;
            occurences = 1;
        }
        lastKnotValue = m_knots.at(i);
    }
}

void Spline::equalizeFirstKnots() {
    real firstOtherKnot = m_knots.at(m_degree);
    real firstKnotsValue = m_knots.at(m_degree - 1);
    if(firstKnotsValue == firstOtherKnot)
        firstKnotsValue -= 1;
    for(uint i = 0; i < m_degree; ++i) {
        m_knots[i] = firstKnotsValue;
    }
}

void Spline::equalizeLastKnots() {
    real lastOtherKnot = m_knots.at(m_controlPoints.size() - 2);
    real lastKnotsValue = m_knots.at(m_controlPoints.size() - 1);
    if(lastOtherKnot == lastKnotsValue)
        lastKnotsValue += 1;
    for(uint i = 0; i < m_degree; ++i) {
        m_knots[m_controlPoints.size() - 1 + i] = lastKnotsValue;
    }
}

void Spline::makeDifferentFirstKnots() {
    real nextValue = m_knots.at(m_degree);
    //do NOT replace int by uint!!!
    for(int i = m_degree - 1; i >= 0; --i) {
        nextValue -= 1.0;
        m_knots[i] = nextValue;
    }
}

void Spline::makeDifferentLastKnots() {
    real lastValue = m_knots.at(m_controlPoints.size() - 2);
    for(uint i = 0; i < m_degree; ++i) {
        lastValue += 1.0;
        m_knots[m_controlPoints.size() - 1 + i] = lastValue;
    }
}

int Spline::pointsWithNormalsThroughOrigin(vector<vec2> &samples, real epsilon) const {
    if((samples.size() < 2 * m_controlPoints.size()) || !isValid())
        return -1;

    //Preparation of the sampleValues vector, which holds evaluable knots and one knot value between
    uint evaluableKnots = m_controlPoints.size() - m_degree;
    vector<real> sampleValues(evaluableKnots * 2 - 1);
    //insert all evaluable knots at every second position of sampleValues
    for(uint i = 0; i < evaluableKnots; ++i) {
        sampleValues[2 * i] = m_knots[i + lowerDomainKnotIndex()];
    }
    //insert an additional value between knot values
    for(uint i = 1; i < sampleValues.size(); i += 2) {
        sampleValues[i] = (sampleValues[i - 1] + sampleValues[i + 1]) / 2.0;
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
                if(b - a < epsilon) {
                    // When there is a corner like it is possible when degree = 1,
                    // the approach above won't work. Therefore this stopping is necessary, too.
                    samples[foundPoints] = evaluate((a + b) / 2);
                    foundPoints += 1;
                    sign = 0;
                    throughOrigin = 1; //leave the while-loop
                }
            }
        }
    }
    return foundPoints;
}

// Returns:  1 ==> point found and inserted in samples with foundPoints increased by 1
//           0 ==> no point found, direction same as in last test
//          -1 ==> no point found, but direction changed => Please check values between!
int Spline::testThroughOrigin(real evaluationValue, int &lastSign, int &foundPoints, vector<vec2> &samples, real epsilon) const {
    vec2 p = evaluate(evaluationValue);
    vec2 n = normal(evaluationValue);
    real direction = p.x * n.y - p.y * n.x;
    int currentSign = (direction < 0) ? -1 : 1;
    if((currentSign > 0 && direction < epsilon) ||
        (currentSign < 0 && (-1.0 * direction) < epsilon)) {
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
