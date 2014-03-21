#include "Spline.h"
#include "helpers.h"

Spline::Spline(int degree) : m_degree(degree), m_isClosed(false), m_tornToEdges(false) {
    if(m_degree <= 0)
        throw;
    for(int i = 0; i <= m_degree; ++i)
        m_knots << i;
}

Spline::Spline(QVector<vec2> interpolationPoints) {
    //TODO!!!
}

Spline::~Spline() {
}

vec2 Spline::evaluate(real value) const {
    int n = lowerNextKnot(value); // value is element of [knot[n], knot[n+1])
    QVector<vec2> controlPointsCopy(m_degree + 1);
    for(int i = 0; i < m_degree + 1; ++i) {
        controlPointsCopy[i] = m_controlPoints.at(n - m_degree + i);
    }
    deBoor(controlPointsCopy, value, n);
    return controlPointsCopy.at(m_degree);
}

vec2 Spline::derivative(real value) const {
    if(!isValid() || value < lowerDomainLimit() || value >= upperDomainLimit())
        return vec2(0, 0);
    //TODO: is is necessary here to test these requirements???

    int n = lowerNextKnot(value); // value is element of [knot[n], knot[n+1])
    QVector<vec2> controlPointsCopy(m_degree + 1);
    for(int i = 0; i < m_degree + 1; ++i) {
        controlPointsCopy[i] = m_controlPoints.at(n - m_degree + i);
    }
    //Stop de Boor algorithm one step earlier than when evaluating a curve point
    deBoor(controlPointsCopy, value, n, m_degree, 1);
    return (vec2(controlPointsCopy.at(m_degree) - controlPointsCopy.at(m_degree - 1))).normalized();
}

void Spline::curve(QVector<QPointF> &curve) const {
    std::cout << "\n\nSPLINE:\n" << (*this) << std::endl;
    if(!isValid())
        return;
    real u = lowerDomainLimit();
    real uStop = upperDomainLimit();
    int iStop = curve.size();

    std::cout << "lowerBorder: " << u << ", upperBorder: " << uStop << std::endl;
    real step = (uStop - u) / iStop;
    for(int i = 0; i < iStop; ++i) {
        if(i == iStop - 1) {
            std::cout << "u in loop would end with: " << u;
            u = uStop - 0.001f;
            std::cout << " and now is set to " << u << std::endl;
        }
        vec2 point = evaluate(u);
        curve[i] = QPointF(point.x(), point.y());
        u += step;
        // update u as floating points are not very accurate
        // but nevertheless the whole spline should be visible:
        if((i & 0xF) == 0) {
            step = (uStop - u) / (iStop - i);
        }
    }
}

int Spline::degree() const {
    return m_degree;
}

void Spline::setDegree(int degree) {
    if(m_degree == degree || degree <= 0)
        return;
    if(m_degree < degree) {
        if(m_tornToEdges && isValid())
            makeDifferentLastKnots();
        for(int i = m_degree; i < degree; ++i) {
            m_knots << (m_knots.at(m_knots.size() - 1) + 1);
            m_degree += 1;
        }
    } else { // m_degree > degree
        for(int i = m_degree; i > degree; --i) {
            m_knots.removeLast();
            m_degree -= 1;
        }
    }
    if(m_tornToEdges) {
        if(isValid())
            equalizeLastKnots();
        else
            adjustKnots();
    }
}

int Spline::numberOfControlPoints() const {
    return m_controlPoints.size();
}

const QVector<vec2>& Spline::controlPoints() const {
    return m_controlPoints;
}

void Spline::addControlPoint(vec2 point) {
    if(m_tornToEdges && isValid()) {
        makeDifferentLastKnots();
        m_knots << (m_knots.at(m_knots.size() - 1) + 1);
        m_controlPoints << point;
        equalizeLastKnots();
    } else {
        m_controlPoints << point;
        m_knots << (m_knots.at(m_knots.size() - 1) + 1);

        if(m_tornToEdges)
            adjustKnots();
    }
}

void Spline::addControlPoint(QPointF point) {
    addControlPoint(vec2(point.x(), point.y()));
}

void Spline::removeControlPoint(int index) {
    if(index < 0 || index >= m_controlPoints.size())
        return;
    m_controlPoints.remove(index);
    m_knots.removeLast();
    if(m_tornToEdges) {
        if(isValid())
            equalizeLastKnots();
        else
            adjustKnots();
    }
    updateClosedCurve();
}

void Spline::moveControlPoint(int index, QPointF newPosition) {
    if(index < 0 || index >= m_controlPoints.size())
        return;
    m_controlPoints[index] = vec2(newPosition.x(), newPosition.y());
}

int Spline::numberOfKnots() const {
    return m_knots.size();
}

const QVector<real>& Spline::knots() const {
    return m_knots;
}

int Spline::multiplicityOfKnotValue(real knotValue) const {
    int count = 0;
    for(int i = 0; i < m_knots.size(); ++i) {
        if(m_knots[i] == knotValue)
            ++count;
    }
    return count;
}

int Spline::multiplicity(int knotIndex) const {
    int count = 1;
    for(int i = knotIndex - 1; i >= 0 && m_knots[i] == m_knots[knotIndex]; --i)
        ++count;
    for(int i = knotIndex + 1; i < m_knots.size() && m_knots[knotIndex] == m_knots[i]; ++i)
        ++count;
    return count;
}

void Spline::knotRefinement(real minDist) {
    if(!isValid())
        return;
    //test each segment from lowest possible u value unto u = m_controlPoints.size() (must not be reached)
    int i = m_degree;
    while(i < m_controlPoints.size() - 1) {
        real dist = m_knots.at(i + 1) - m_knots.at(i);
        if(dist > minDist) {
            real newValue = m_knots.at(i) + 0.5 * dist;
            refineAt(newValue);
        } else {
            ++i;
        }
    }
}

//TODO: in description: check must done before and by yourself!!!
void Spline::refineAt(real knotValue, int multiplicity) {
    //A multiplicity of 0 would mean no knot insertion and
    //an insertion with multiplicity > m_degree would lead to a whole in the spline curve!
    if(multiplicity <= 0 || multiplicity > m_degree)
        return;
    //Use de Boor scheme to geht the new point(s)
    int n = lowerNextKnot(knotValue); // knotValue is element of [knot[n], knot[n+1])
    QVector<vec2> controlPointsCopy(m_degree + 1);
    for(int i = 0; i <= m_degree; ++i) {
        controlPointsCopy[i] = m_controlPoints.at(n - m_degree + i);
    }

    QVector<vec2> newPoints(m_degree + multiplicity - 1);

    //When the new knotVaue is inserted r-times (multiplicity = r) the new points are equal
    //to the points of the de boor scheme in the r-th column and the outer way to this column
    for(int i = 1; i < multiplicity; ++i) {

        //do only one step of the de boor algorithm each iteration
        //and only multiplicity times
        //(so for multiplicity == 1 only one step is done)
        deBoor(controlPointsCopy, knotValue, n, m_degree - i + 1, m_degree - i);

        if(i == multiplicity) {
            //copy all up to now calculated points to the new points
            //except first one, which didn't change (exists already in spline)
            for(int j = 1; j <= m_degree; ++j) {
                newPoints[j - 1] = controlPointsCopy.at(j);
            }
        } else {
            //copy only last point, as it will be overwritten in next deBoor step
            newPoints[newPoints.size() - i] = controlPointsCopy.last();
        }
    }
    //copy now new points into control points vector:
    m_controlPoints.resize(m_controlPoints.size() + multiplicity);
    int i = m_controlPoints.size() - 1 - multiplicity;
    for(; i >= n; --i) {
        m_controlPoints[i + multiplicity] = m_controlPoints.at(i);
    }
    int j = newPoints.size();
    for(; i > n - m_degree; --i) {
        m_controlPoints[i + multiplicity] = newPoints[--j];
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
        if(m_tornToEdges) {
            if(isValid()) {
                equalizeFirstKnots();
                equalizeLastKnots();
            } else {
                adjustKnots();
            }
        } else { // not torn to edges
            if(isValid()) {
                makeDifferentFirstKnots();
                makeDifferentLastKnots();
            } else {
                adjustKnots();
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
    for(int i = 1; i <= m_degree; ++i) {
        if(m_knots.at(i) != start
            || m_knots.at(m_controlPoints.size() - 1 + i) != stop)
            return false;
    }
    return true;
}

void Spline::updateTornToEdges() {
    m_tornToEdges = checkTornToEdges();
}

bool Spline::isValidMessages(std::ostream &os) const {
    bool isValid = true;
    if(m_controlPoints.size() != m_knots.size() - m_degree - 1) {
        os << "Number of control points does not suite to number of knots and degree\n";
        os << "number of control points should be " << (m_knots.size() - m_degree - 1);
        os << "but is " << m_controlPoints.size() << std::endl;
        isValid = false;
    }
    int occurences = 1;
    real lastValue = m_knots.at(0);
    for(int i = 1; i < m_knots.size(); ++i) {
        if(lastValue == m_knots.at(i)) {
            ++occurences;
        } else if (lastValue > m_knots.at(i)) {
            os << "Knots are not sorted in ascending order at knot" << i-1 << " to " << i << std::endl;
            isValid = false;
        } else {
            lastValue = m_knots.at(i);
            occurences = 1;
        }
        if((occurences > m_degree && (i != m_degree || i != m_knots.size() - 1))
            || occurences > (m_degree + 1)) {
            os << "Multiplicity of knot " << i << " is too high (" << occurences << ")." << std::endl;
        }
    }
    return isValid;
}

bool Spline::isValid() const {
    if(m_degree <= 0 || m_knots.size() < 2)
        return false;
    if(m_controlPoints.size() != m_knots.size() - m_degree - 1)
        return false;
    return m_controlPoints.size() > m_degree; //first visible for degree + 1 control points
}

real Spline::lowerDomainLimit() const {
    if(!isValid())
        return -1.0; //No curve available at the moment!
    return m_knots[m_degree];
}
real Spline::upperDomainLimit() const {
    if(!isValid())
        return -1.0; //No curve available at the moment!
    return m_knots[m_controlPoints.size()]; //is equal to: m_knots[m_knots.size() - 1 - m_degree]
}

int Spline::lowerNextKnot(real value) const {
    for(int i = 0; i < m_knots.size(); ++i) { //TODO: can be optimized (suchen erst ab m_degree...)
        if(m_knots[i] > value)
            return i - 1;
    }
    return m_knots.size() - 1;
}


//TODO: methode anschaun und untersuchen:
void Spline::getIntersectionPointsWithRay(const Ray &ray, QVector<vec2> &intersectionPoints) const {
    // intersectionPoints.resize(0); //ensure that no wrong values are returned

    // //first test the control point polygon.
    // //If the ray has no intersection with it, there can't be an intersection with the spline curve, too.
    // for(int i = 0; i < m_controlPoints.size() - 1; ++i) {
    //     vec2 startLine, endLine, intersectionPoint;
    //     startLine = m_controlPoints[i];
    //     endLine = m_controlPoints[i + 1];

    //     if(ray.intersect(startLine, endLine, intersectionPoint)) {

    //         //An intersection occured with a line of the control point polygon
    //         //When degree = 1, the spline curve is equal to the control point polygon
    //         //So return the found point.
    //         if(m_degree == 1) {
    //             intersectionPoints.push_back(intersectionPoint);
    //         }
    //         //For higher degrees ???
    //         else {
    //             QVector<real> knots(m_knots);
    //             std::vector<vec2> points(m_controlPoints);

    //             int startKnotIndex = i - (m_degree - 1);
    //             if(startKnotIndex < m_degree)
    //                 startKnotIndex = m_degree;
    //             int stopKnotIndex = i + m_degree + 1 + m_degree + 1;
    //             if(stopKnotIndex > points.size())
    //                 stopKnotIndex = points.size();
    //             bool newPointsInserted = false;
    //             do {
    //                 std::vector<real> newKnots;
    //                 for(int j = startKnotIndex; j < stopKnotIndex; ++j) {
    //                     if(knots[j + 1] - knots[j] > 0.05f) {
    //                         real newKnot = knots[j] + (knots[j + 1] - knots[j]) / 2.0f;
    //                         newKnots.push_back(newKnot);
    //                     }
    //                 }
    //                 stopKnotIndex += newKnots.size();
    //                 newPointsInserted = !newKnots.empty();
    //                 if(newPointsInserted)
    //                     refine(knots, points, newKnots);
    //             } while(newPointsInserted);

    //             for(int j = startKnotIndex; j < stopKnotIndex - m_degree - 1; ++j) {
    //                 vec2 startLine, endLine, intersectionPoint;
    //                 startLine = vec2(points[j]);
    //                 endLine = vec2(points[j + 1]);
    //                 if(ray.intersect(startLine, endLine, intersectionPoint)) {
    //                     intersectionPoints.push_back(intersectionPoint);
    //                 }
    //             }
    //         }
    //     }
    // }
}

void Spline::deBoor(QVector<vec2> &controlPoints, real value, real n) const {
    deBoor(controlPoints, value, n, m_degree, 0);
}

void Spline::deBoor(QVector<vec2> &controlPoints, real value, real n, int degree, int stop) const {
    assert(controlPoints.size() >= degree + 1 && degree >= 0 && stop >= 0);
    int lastIndex = controlPoints.size() - 1;

    for(int i = n, index = lastIndex; i > n - degree; --i, --index) {
        real alpha = (value - m_knots.at(i)) / (m_knots.at(i + degree) - m_knots.at(i));
        controlPoints[index] =
            (1.0f - alpha) * controlPoints.at(index - 1)
                + alpha * controlPoints.at(index);
    }
    if(stop != degree)
        deBoor(controlPoints, value, n, --degree, stop);
}

void Spline::adjustKnots() {
    int lastKnotValue = m_knots[0];
    int occurences = 1;
    for(int i = 1; i < m_knots.size(); ++i) {

        //when torn to edges the first n = m_degree + 1 knots have to have the same value
        if(m_tornToEdges && i <= m_degree) {
            m_knots[i] = lastKnotValue;
            ++occurences;
        } else
        //when torn to edges the last knots have to have the same value
        //starting with knot[m_controlPoints.size()]. If this knot has same value
        //as the one before, give it a new bigger value.
        if(m_tornToEdges && i == m_controlPoints.size()) {
            if(m_knots.at(i) == lastKnotValue)
                m_knots[i] = lastKnotValue + 1;
            occurences = 1;
        } else

        //when torn to edges the last n = m_degree + 1 (= m_knots.size() - m_controlPoints.size())
        //knots need the same value
        if(m_tornToEdges && i > m_controlPoints.size()) {
            assert(occurences <= m_degree); //otherwise, number of control points to knots and to degree is not correct!
            m_knots[i] = lastKnotValue;
            ++occurences;
        } else

        //assure that there are not more than n = m_degree + 1 same knot values
        if(m_knots.at(i) == lastKnotValue) {
            ++occurences;
            if(occurences > m_degree + 1) {
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
    real firstOtherKnot = m_knots.at(m_degree + 1);
    real firstKnotsValue = m_knots.at(m_degree);
    if(firstKnotsValue == firstOtherKnot)
        firstKnotsValue -= 1;
    for(int i = 0; i <= m_degree; ++i) {
        m_knots[i] = firstKnotsValue;
    }
}

void Spline::equalizeLastKnots() {
    real lastOtherKnot = m_knots.at(m_controlPoints.size() - 1);
    real lastKnotsValue = m_knots.at(m_controlPoints.size());
    if(lastOtherKnot == lastKnotsValue)
        lastKnotsValue += 1;
    for(int i = 0; i <= m_degree; ++i) {
        m_knots[m_controlPoints.size() + i] =lastKnotsValue;
    }
}

void Spline::makeDifferentFirstKnots() {
    real nextValue = m_knots.at(m_degree + 1);
    for(int i = m_degree; i >= 0; --i) {
        nextValue -= 1.0f;
        m_knots[i] = nextValue;
    }
}

void Spline::makeDifferentLastKnots() {
    real lastValue = m_knots.at(m_controlPoints.size() - 1);
    for (int i = 0; i <= m_degree; ++i) {
        lastValue += 1.0f;
        m_knots[m_controlPoints.size() + i] = lastValue;
    }
}
