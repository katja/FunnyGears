#include "Spline.h"
#include "helpers.h"

Spline::Spline() : m_degree(2), m_tornToEdges(false) {
    m_knots << 0 << 1 << 2;
}

Spline::~Spline() {
}

vec2 Spline::evaluate(qreal value) const {
    if(!isValid() || value < lowerDomainBorder() || value > upperDomainBorder())
        return vec2(0, 0); //TODO: is this REALLY necessary or only time consuming?????
    int n = lowerNextKnot(value); // value is element of knot[n] and knot[n+1]
    QVector<vec2> controlPointsCopy(m_degree + 1);
    for(int i = 0; i < m_degree + 1; ++i) {
        controlPointsCopy[i] = m_controlPoints.at(n - m_degree + i);
    }
    deBoor(controlPointsCopy, value, n);
    return controlPointsCopy.at(m_degree);
}

void Spline::curve(QVector<QPointF> &curve) const {
    std::cout << "\n\nSPLINE:\n" << (*this) << std::endl;
    qreal u = lowerDomainBorder();
    qreal uStop = upperDomainBorder();
    int iStop = curve.size();

    std::cout << "lowerBorder: " << u << ", upperBorder: " << uStop << std::endl;
    qreal step = (uStop - u) / iStop;
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
    if(degree < 0 || m_degree == degree)
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

const QVector<vec2>& Spline::controlPoints() const {
    return m_controlPoints;
}

const QVector<real>& Spline::knots() const {
    return m_knots;
}

bool Spline::isValid() const {
    return m_controlPoints.size() > m_degree; //first visible for degree + 1 control points
}

qreal Spline::lowerDomainBorder() const {
    if(!isValid())
        return -1.0; //No curve available at the moment!
    return m_knots[m_degree];
}
qreal Spline::upperDomainBorder() const {
    if(!isValid())
        return -1.0; //No curve available at the moment!
    return m_knots[m_controlPoints.size()]; //is equal to: m_knots[m_knots.size() - 1 - m_degree]
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

void Spline::moveControlPoint(int index, QPointF newPosition) {
    if(index < 0 || index >= m_controlPoints.size())
        return;
    m_controlPoints[index] = vec2(newPosition.x(), newPosition.y());
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

int Spline::lowerNextKnot(int value) const {
    for(int i = 0; i < m_knots.size(); ++i) { //TODO: can be optimized (suchen erst ab m_degree...)
        if(m_knots[i] > value)
            return i - 1;
    }
    return m_knots.size() - 1;
}

void Spline::deBoor(QVector<vec2> &controlPoints, qreal value, qreal n) const {
    deBoor(controlPoints, value, n, m_degree, 0);
}

void Spline::deBoor(QVector<vec2> &controlPoints, qreal value, qreal n, int degree, int stop) const {
    assert(controlPoints.size() >= degree + 1);
    int lastIndex = controlPoints.size() - 1;

    for(int i = n, index = lastIndex; i > n - degree; --i, --index) {
        qreal alpha = (value - m_knots.at(i)) / (m_knots.at(i + degree) - m_knots.at(i));
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

// void Spline::updateKnots() {
//     if(m_tornToEdges) {
//         equalizeLastKnots();
//         equalizeFirstKnots();
//     } else {
//         makeDifferentLastKnots();
//         makeDifferentFirstKnots();
//     }
// }

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

