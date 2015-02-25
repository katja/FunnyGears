#include "ContactPointIterator.h"

ContactPointIterator::ContactPointIterator() :
    m_cpList(nullptr),
    m_ncp(nullptr)
{}

ContactPointIterator::~ContactPointIterator() {}

ContactPointIterator& ContactPointIterator::operator++() {
    m_previousPoint = currentPoint();

    if(m_iterationState == IterationState::CP) {
        if(m_iterationDirection == IterationDirection::Normal)
            ++m_cpIterator; else --m_cpIterator;

    } else {
        if(m_iterationDirection == IterationDirection::Normal)
            ++m_ncpPosition; else --m_ncpPosition;
    }
    return (*this);
}

bool ContactPointIterator::reachedEnd() const {
    if(m_iterationState == IterationState::CP) {
        if(m_iterationDirection == IterationDirection::Normal)
            return (m_cpIterator == m_cpList->end());
        else
            return (m_cpIterator + 1 == m_cpList->begin());

    } else {
        if(m_iterationDirection == IterationDirection::Normal)
            return ((uint)m_ncpPosition >= m_ncp->points.size());
        else
            return (m_ncpPosition <= -1);
    }
}

void ContactPointIterator::startWith(vector<ContactPoint*>::iterator firstPoint, vector<ContactPoint*> *firstList) {
    m_cpIterator = firstPoint;
    m_cpList = firstList;
    setNormalDirection();
    m_iterationDirection = IterationDirection::Normal;
    m_iterationLocation = IterationLocation::Ground;
    m_iterationState = IterationState::CP;
}

vec2 ContactPointIterator::currentPoint() const {
    if(m_iterationState == IterationState::CP) {
        if(m_iterationLocation == IterationLocation::Ground)
            return (*m_cpIterator)->point;
        else
            return (*m_cpIterator)->forbiddenAreaEndPoint;
    } else {
        if(m_iterationLocation == IterationLocation::Ground)
            return m_ncp->points[m_ncpPosition];
        else
            return forbiddenAreaEndPoint(m_ncp, m_ncpPosition);
    }
}

vec2 ContactPointIterator::previousPoint() const {
    return m_previousPoint;
}

ContactPoint* ContactPointIterator::currentCP() const {
    if(m_iterationState == IterationState::CP)
        return (*m_cpIterator);
    else
        return m_ncp;
}

void ContactPointIterator::continueWith(CPcutting cutting) {
    vec2 infrontOfCutting;
    if(cutting.location == IterationLocation::Ground)
        infrontOfCutting = (*(cutting.placeBeforeCutting))->point;
    else
        infrontOfCutting = (*(cutting.placeBeforeCutting))->forbiddenAreaEndPoint;

    if(isOnNormalsSide(infrontOfCutting)) {
        //"normal" walking direction, set new current point to point after cutting
        m_iterationDirection = IterationDirection::Normal;
        m_cpIterator = (cutting.placeBeforeCutting + 1);

    } else {
        //"reverse" walking direction, set new current point to point before cutting
        m_iterationDirection = IterationDirection::Reverse;
        m_cpIterator = cutting.placeBeforeCutting;
    }

    m_iterationState = IterationState::CP;
    m_iterationLocation = cutting.location;
    m_cpList = cutting.list;
    m_previousPoint = cutting.cuttingPoint;
}

void ContactPointIterator::continueWith(NCPcutting cutting) {
    vec2 infrontOfCutting;
    if(cutting.location == IterationLocation::Ground)
        infrontOfCutting = cutting.ncp->points[cutting.placeBeforeCutting];
    else
        infrontOfCutting = forbiddenAreaEndPoint(cutting.ncp, cutting.placeBeforeCutting);

    if(isOnNormalsSide(infrontOfCutting)) {
        //"normal" walking direction, set new current point to point after cutting
        m_iterationDirection = IterationDirection::Normal;
        m_ncpPosition = cutting.placeBeforeCutting + 1;

    } else {
        //"reverse" walking direction, set new current point to point before cutting
        m_iterationDirection = IterationDirection::Reverse;
        m_ncpPosition = cutting.placeBeforeCutting;
    }

    m_iterationState = IterationState::NCP;
    m_iterationLocation = cutting.location;
    m_ncp = cutting.ncp;
    m_previousPoint = cutting.cuttingPoint;
}

bool ContactPointIterator::belongsToQuad(ContactPoint *a, ContactPoint *b) const {
    if(m_iterationState == IterationState::NCP)
        return false;

    if(m_iterationDirection == IterationDirection::Normal)
        return *m_cpIterator == a || *m_cpIterator == b || *(m_cpIterator - 1) == a || *(m_cpIterator - 1) == b;
    else
        return *m_cpIterator == a || *m_cpIterator == b || *(m_cpIterator + 1) == a || *(m_cpIterator + 1) == b;

    // ContactPoint *wannabePrevious = nullptr;

    // if(*m_cpIterator == a)
    //     wannabePrevious = b;
    // else if(*m_cpIterator == b)
    //     wannabePrevious = a;
    // else
    //     return false;

    // if((m_iterationDirection == IterationDirection::Normal && (*(m_cpIterator - 1) == wannabePrevious))
    //     || (m_iterationDirection == IterationDirection::Reverse && (*(m_cpIterator + 1) == wannabePrevious)))
    //     return true;
    // return false;
}

bool ContactPointIterator::belongsToQuad(NoneContactPoint *ncp, uint a, uint b) const {
    if(m_iterationState == IterationState::CP || ncp != m_ncp)
        return false;

    if(m_iterationDirection == IterationDirection::Normal)
        return (uint)m_ncpPosition == a || (uint)m_ncpPosition == b || (uint)m_ncpPosition - 1 == a || (uint)m_ncpPosition - 1 == b;
    else
        return (uint)m_ncpPosition == a || (uint)m_ncpPosition == b || (uint)m_ncpPosition + 1 == a || (uint)m_ncpPosition + 1 == b;

    // uint wannabePrevious;
    // if((uint)m_ncpPosition == a)
    //     wannabePrevious = b;
    // else if((uint)m_ncpPosition == b)
    //     wannabePrevious = a;
    // else
    //     return false;

    // if((m_iterationDirection == IterationDirection::Normal && (uint)(m_ncpPosition - 1) == wannabePrevious) ||
    //     (m_iterationDirection == IterationDirection::Reverse && (uint)(m_ncpPosition + 1) == wannabePrevious))
    //     return true;
    // return false;
}

std::ostream& ContactPointIterator::currentStateStream(std::ostream &os) const {
    os << "ContactPointIterator with properties... \n";

    os << "Iteration state:      ";
    if(m_iterationState == IterationState::CP)
        os << "CP" << std::endl;
    else
        os << "NCP" << std::endl;

    os << "Iteration direction:  ";
    if(m_iterationDirection == IterationDirection::Normal)
        os << "Normal" << std::endl;
    else
        os << "Reverse" << std::endl;

    os << "Iteration location:   ";
    if(m_iterationLocation == IterationLocation::Ground)
        os << "Ground" << std::endl;
    else
        os << "Top" << std::endl;

    os << "Normal direction:     " << m_normalDirection << std::endl;
    os << "Previous point:       " << m_previousPoint << std::endl;
    os << "Current CP pointer:   " << *(m_cpIterator) << std::endl;
    os << "Current CP list:      " << m_cpList << std::endl;
    os << "Current NCP pointer:  " << m_ncp << std::endl;
    os << "Current NCP position: " << m_ncpPosition << std::endl;

    return os;
}

void ContactPointIterator::setNormalDirection() {
    if(m_cpIterator + 1 == m_cpList->end()) {
        std::cerr << "NORMAL DIRECTION COULD NOT BE SET!!!" << std::endl;
        return; // has no predecessor
    }
    vec2 toNextPoint = (*(m_cpIterator + 1))->point - (*m_cpIterator)->point;
    vec2 normal = (*m_cpIterator)->normal;
    real direction = cross(normalize(toNextPoint), normal);
    if(direction > 0.0)
        m_normalDirection = 1;
    else
        m_normalDirection = -1;
}

bool ContactPointIterator::isOnNormalsSide(vec2 point) const {
    assert(m_normalDirection != 0);
    vec2 toCurrentPoint = currentPoint() - m_previousPoint;
    vec2 toPoint = point - m_previousPoint;
    real side = cross(normalize(toCurrentPoint), normalize(toPoint));
    return (m_normalDirection == 1 && side > 0.0) || (m_normalDirection == -1 && side < 0.0);
}

vec2 ContactPointIterator::forbiddenAreaEndPoint(NoneContactPoint *ncp, uint position) const {
    return ncp->points[position] + ncp->forbiddenAreaLength * ncp->normals[position];
}
