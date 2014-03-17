#include "Spline.h"

Spline::Spline() : m_degree(2), m_tornToEdges(false) {
    m_knots << 0 << 1 << 2 << 3;
}

Spline::~Spline() {
}

vec2 Spline::evaluate(qreal value) const {
    if(!enoughControlPoints() || value < lowerDomainBorder() || value > upperDomainBorder())
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
    qreal u = lowerDomainBorder();
    qreal uStop = upperDomainBorder();
    int iStop = curve.size();

    qreal step = (uStop - u) / iStop;
    for(int i = 0; i < iStop; ++i) {
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
        for(int i = m_degree; i < degree; ++i) {
            m_knots << (m_knots.at(m_knots.size() - 1) + 1);
            m_knots.prepend(m_knots.at(0) - 1);
            m_degree += 1;
        }
    } else { // m_degree > degree
        for(int i = m_degree; i > degree; --i) {
            m_knots.removeFirst();
            m_knots.removeLast();
            m_degree -= 1;
        }
    }
}

bool Spline::enoughControlPoints() const {
    return m_controlPoints.size() >= 1 + m_degree; //first visible for degree + 1 control points
}

qreal Spline::lowerDomainBorder() const {
    if(!enoughControlPoints())
        return -1.0; //No curve available at the moment!
    return m_knots[m_degree];
}
qreal Spline::upperDomainBorder() const {
    if(!enoughControlPoints())
        return -1.0; //No curve available at the moment!
    return m_knots[m_controlPoints.size()];
}

void Spline::addControlPoint(vec2 point) {
    m_controlPoints << point;
    if(!m_tornToEdges) {
        m_knots << (m_knots[m_knots.size() - 1] + 1); // append a knot with a value "1" higher than previous end knot
    } else {
        if(m_knots.size() <= m_degree) { //new knot needs same value as all knots up to now!
            m_knots << m_knots[m_knots.size() - 1];
        } else if(m_knots.size() == m_degree + 1) { //new knot is first with other value
            m_knots << (m_knots[m_knots.size() - 1] + 1);
        } else if(m_knots.size() < 2 * (m_degree + 1)) { //new knot belongs to the last knots, and yet there are not enough knots available. => we can add it simply at the end
            m_knots << m_knots[m_knots.size() - 1];
        } else { //new knot belongs to the last knots and as not more than m_degree same knots are possible, we have to increase last knots.
            qreal newKnotValue = m_knots[m_knots.size() - 1];
            for(int i = 0; i < m_degree; ++i) {
                m_knots[m_knots.size() - 1 - i] = newKnotValue;
            }
            m_knots << newKnotValue;
        }
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
    if(m_tornToEdges != tearToEdges)
    m_tornToEdges = tearToEdges;
    if(m_tornToEdges) {
        equalizeLastKnots();
        equalizeFirstKnots();
    } else {
        makeDifferentLastKnots();
        makeDifferentFirstKnots();
    }
}

bool Spline::isTornToEdges() {
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

void Spline::equalizeFirstKnots() {
    for(int i = 0; i < m_degree; ++i) {
        m_knots[i] = m_knots.at(m_degree);
    }
}

void Spline::equalizeLastKnots() {
    int lastIndex = m_knots.size() - 1;
    for(int i = 0; i < m_degree; ++i) {
        m_knots[lastIndex - i] = m_knots[lastIndex - m_degree];
    }
}

void Spline::makeDifferentFirstKnots() {
    for(int i = m_degree - 1; i >= 0; --i) {
        m_knots[i] = m_knots.at(i + 1) - 1;
    }
}

void Spline::makeDifferentLastKnots() {
    int refIndex = m_knots.size() - m_degree;
    for (int i = refIndex; i < refIndex + m_degree; ++i) {
        m_knots[i] = m_knots.at(i - 1) + 1;
    }
}

