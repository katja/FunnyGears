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
            ++m_cpIterator;
        else
            --m_cpIterator;

    } else { //IterationState::NCP
        if(m_iterationDirection == IterationDirection::Normal)
            ++m_ncpPosition;
        else
            --m_ncpPosition;
        m_ncp->examinedIndex = m_ncpPosition;
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

bool ContactPointIterator::tryToContinueWithOtherList(list<ContactPointsWithPosition*> &possibleLists) {
    //Normally one should not get cases, where the end of a list is reached, when in NCP state.
    //Therefore it is not regarded as important to switch to another list
    if(m_iterationState == IterationState::NCP)
        return false;

    //Up to now, there is only the case regarded with IterationLocation::Ground and IterationDirection::Normal
    //as this will be the most probable
    if(m_iterationLocation != IterationLocation::Ground || m_iterationDirection != IterationDirection::Normal)
        return false; //only yet implemented Case uses: IterationLocation::Ground and IterationDirection::Normal

    for(ContactPointsWithPosition *contactPointsWithPosition : possibleLists) {
        vector<ContactPoint*> *cpList = &(contactPointsWithPosition->points);
        if(cpList != m_cpList) {
            vec2 pointDistance = cpList->front()->point - (*(m_cpIterator - 1))->point;
            vec2 forbiddenEndDistance = cpList->front()->forbiddenAreaEndPoint - (*(m_cpIterator - 1))->forbiddenAreaEndPoint;
            if(glm::length(pointDistance) < 0.5 && glm::length(forbiddenEndDistance) < 0.5) {
                m_cpList = cpList;
                m_cpIterator = cpList->begin();
                return true;
            }
        }
    }
    return false;
}

void ContactPointIterator::startWith(CPcutting cutting, int standardNormalDirection) {
    m_normalDirection = standardNormalDirection;
    m_iterationState = IterationState::CP;
    m_cpList = cutting.list;
    m_previousPoint = cutting.cuttingPoint;

    vec2 normal, toNextPoint;

    if(cutting.location == IterationLocation::Ground) {
        m_iterationLocation = IterationLocation::Ground;
        normal = (*(cutting.placeBeforeCutting))->normal;
        toNextPoint = (*(cutting.placeBeforeCutting + 1))->point
                            - (*(cutting.placeBeforeCutting))->point;

    } else { //cutting.location == IterationLocation::Top
        m_iterationLocation = IterationLocation::Top;
        normal = (*(cutting.placeBeforeCutting))->normal * vec2(-1, -1); //reverse direction as we are on top
        toNextPoint = (*(cutting.placeBeforeCutting + 1))->forbiddenAreaEndPoint
                            - (*(cutting.placeBeforeCutting))->forbiddenAreaEndPoint;
    }
    real direction = cross(normalize(toNextPoint), normal);

    if((direction > 0.0 && m_normalDirection == 1) ||
        (direction < 0.0 && m_normalDirection == -1)) {
        m_cpIterator = (cutting.placeBeforeCutting + 1);
        m_iterationDirection = IterationDirection::Normal;
    } else {
        m_cpIterator = (cutting.placeBeforeCutting);
        m_iterationDirection = IterationDirection::Reverse;
    }
}

void ContactPointIterator::startWith(NCPcutting cutting, int standardNormalDirection) {
    m_normalDirection = standardNormalDirection;
    m_iterationState = IterationState::NCP;
    m_ncp = cutting.ncp;
    m_previousPoint = cutting.cuttingPoint;

    vec2 normal, toNextPoint;

    if(cutting.location == IterationLocation::Ground) {
        m_iterationLocation = IterationLocation::Ground;
        normal = m_ncp->normals[cutting.placeBeforeCutting];
        toNextPoint = m_ncp->points[cutting.placeBeforeCutting + 1]
                        - m_ncp->points[cutting.placeBeforeCutting];

    } else { //cutting.location == IterationLocation::Top
        m_iterationLocation = IterationLocation::Top;
        normal = m_ncp->normals[cutting.placeBeforeCutting] * vec2(-1, -1); //reverse direction as we are on top
        vec2 pointBefore = forbiddenAreaEndPoint(m_ncp, cutting.placeBeforeCutting);
        vec2 pointAfter = forbiddenAreaEndPoint(m_ncp, cutting.placeBeforeCutting + 1);
        toNextPoint = pointAfter - pointBefore;
    }
    real direction = cross(normalize(toNextPoint), normal);

    if((direction > 0.0 && m_normalDirection == 1) ||
        (direction < 0.0 && m_normalDirection == -1)) {
        m_ncpPosition = cutting.placeBeforeCutting + 1;
        m_ncp->examinedIndex = m_ncpPosition;
        m_iterationDirection = IterationDirection::Normal;
    } else {
        m_ncpPosition = cutting.placeBeforeCutting;
        m_ncp->examinedIndex = m_ncpPosition;
        m_iterationDirection = IterationDirection::Reverse;
    }
}

vec2 ContactPointIterator::currentPoint() const {
    if(m_iterationState == IterationState::CP) {
        if(m_iterationLocation == IterationLocation::Ground)
            return (*m_cpIterator)->point;
        else
            return (*m_cpIterator)->forbiddenAreaEndPoint;

    } else { //IterationState::NCP
        if(m_iterationLocation == IterationLocation::Ground)
            return m_ncp->points[m_ncpPosition];
        else
            return forbiddenAreaEndPoint(m_ncp, m_ncpPosition);
    }
}

vec2 ContactPointIterator::previousPoint() const {
    return m_previousPoint;
}

bool ContactPointIterator::isCurrentlyInCorrectCP() const {
    return (m_iterationState == IterationState::CP
        && m_iterationLocation == IterationLocation::Ground);
}

bool ContactPointIterator::isCurrentlyInCorrectNCP() const {
    return (m_iterationState == IterationState::NCP
        && m_iterationLocation == IterationLocation::Ground);
}

ContactPoint* ContactPointIterator::currentCP() const {
    if(m_iterationState == IterationState::CP)
        return *m_cpIterator;
    else
        return nullptr;
}

ContactPoint* ContactPointIterator::previousCP() const {
    if(m_iterationState != IterationState::CP)
        return nullptr;
    if(m_iterationDirection == IterationDirection::Normal)
        return *(m_cpIterator - 1);
    else
        return *(m_cpIterator + 1);
}

NoneContactPoint* ContactPointIterator::currentNCP() const {
    if(m_iterationState == IterationState::NCP)
        return m_ncp;
    else
        return nullptr;
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
    m_ncp->examinedIndex = m_ncpPosition;
    m_previousPoint = cutting.cuttingPoint;
}

void ContactPointIterator::switchTo(CPcutting cutting) {
    vec2 infrontOfCutting;
    if(cutting.location == IterationLocation::Ground)
        infrontOfCutting = (*(cutting.placeBeforeCutting))->point;
    else
        infrontOfCutting = (*(cutting.placeBeforeCutting))->forbiddenAreaEndPoint;

    vec2 previousNormalEnd;
    if(m_iterationState == IterationState::CP) {
        previousNormalEnd = previousCP()->point;
        if(m_iterationLocation == IterationLocation::Top)
            previousNormalEnd = previousCP()->forbiddenAreaEndPoint;
    } else { //IterationState::NCP
        int toPreviousOffset = (m_iterationDirection == IterationDirection::Normal) ? -1 : +1;
        previousNormalEnd = m_ncp->points[m_ncpPosition + toPreviousOffset];
        if(m_iterationLocation == IterationLocation::Top)
            previousNormalEnd = forbiddenAreaEndPoint(m_ncp, m_ncpPosition + toPreviousOffset);
    }

    if(isOnNormalsSideOf(m_previousPoint, previousNormalEnd, infrontOfCutting)) {
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

void ContactPointIterator::switchTo(NCPcutting cutting) {
    vec2 infrontOfCutting;
    if(cutting.location == IterationLocation::Ground)
        infrontOfCutting = cutting.ncp->points[cutting.placeBeforeCutting];
    else
        infrontOfCutting = forbiddenAreaEndPoint(cutting.ncp, cutting.placeBeforeCutting);

    vec2 previousNormalEnd;
    if(m_iterationState == IterationState::CP) {
        previousNormalEnd = previousCP()->point;
        if(m_iterationLocation == IterationLocation::Top)
            previousNormalEnd = previousCP()->forbiddenAreaEndPoint;
    } else { //IterationState::NCP
        int toPreviousOffset = (m_iterationDirection == IterationDirection::Normal) ? -1 : +1;
        previousNormalEnd = m_ncp->points[m_ncpPosition + toPreviousOffset];
        if(m_iterationLocation == IterationLocation::Top)
            previousNormalEnd = forbiddenAreaEndPoint(m_ncp, m_ncpPosition + toPreviousOffset);
    }

    if(isOnNormalsSideOf(m_previousPoint, previousNormalEnd, infrontOfCutting)) {
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
    m_ncp->examinedIndex = m_ncpPosition;
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
    vec2 toCurrentPoint = currentPoint() - m_previousPoint;
    vec2 toPoint = point - m_previousPoint;
    real side = cross(normalize(toCurrentPoint), normalize(toPoint));
    return (m_normalDirection == 1 && side > 0.0) || (m_normalDirection == -1 && side < 0.0);
}

bool ContactPointIterator::isOnNormalsSideOf(vec2 lineStart, vec2 lineEnd, vec2 point) const {
    vec2 toLineEnd = lineEnd - lineStart;
    vec2 toPoint = point - lineStart;
    real side = cross(normalize(toLineEnd), normalize(toPoint));
    return (m_normalDirection == 1 && side > 0.0) || (m_normalDirection == -1 && side < 0.0);
}

vec2 ContactPointIterator::forbiddenAreaEndPoint(NoneContactPoint *ncp, uint position) const {
    return ncp->points[position] + ncp->forbiddenAreaLength * ncp->normals[position];
}
